/*
 *Board: Adafruit Feather M0 RFM9x LoRa 433MHz
 *Might be a good read: https://learn.adafruit.com/adafruit-feather-m0-bluefruit-le/adapting-sketches-to-m0
 *Libraries MutichannelGasSensor.h and MutichannelGasSensor.cpp both adapted for Adafruit SAMD M0 boards (serial vs serialusb)
*/

#include <Wire.h>                     //Needed for I2C 
#include "MutichannelGasSensor.h"     //Needed for gas sensor

//For LoRa 
#include <LoRa.h>                     //Needed for LoRa
#define RFM95_SS 8                    //The CS pin (#8) does not have a pullup built in so be sure to set this pin HIGH when not using the radio!
#define RFM95_RST 4
#define RFM95_INT 3

//For gas sensor
float ValueNH3;
float ValueCO;
float ValueNO2;
float ValueC3H8;
float ValueC4H10;
float ValueCH4;
float ValueH2;
float ValueC2H5OH;

byte localAddress = 05;              // address of this device (0x5 is HEX "5")
byte webGatewayAddress = 0x31;        // address of web gateway
byte broadcastAddress = 0xFF;         // broadcast address
byte destination = 0xFF;              // destination to send to
byte msgCount = 0;                    // count of outgoing messages
bool expectingMessage = false;        //Indicates alarm sent, ack needed
bool messageReceived = false;         //Indicates a properly-formatted–but not necessarily appropriate–message received
String outgoingMsg;                   //contents of outgoing radio transmission
String LastReceivedTrans;             //Record last message received by this station
String LastSentTrans;                 //Record last transmission sent from this station
byte fullpayload[8];                      //Byte array to store data for transmission https://www.thethingsnetwork.org/docs/devices/bytes.html
int sizeofFullPayload;
bool transmitRequested = 0;

bool debug = true;                    //To enable debugging
bool debugPrinted = false;            //track if we've printed debug data (don't spam serial console)
String debugMessage;                  //optional message to add to displayDebug()

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(1000);
  if ( Serial ) Serial.println("Serial enabled");
  
  //--------- Lora Settings
  //setup LoRa transceiver module
  //LoRa.setPins(csPin, resetPin, irqPin);// set CS, reset, IRQ pin
  LoRa.setPins(RFM95_SS, RFM95_RST, RFM95_INT);
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia (433E6 for MCW home)
  //866E6 for Europe
  //915E6 for North America
  if ( Serial ) Serial.println("Initializing LoRa");
  while (!LoRa.begin(433E6)) {
    if ( Serial ) Serial.print(".");
    delay(500);
  }
  if ( Serial ) Serial.println("LoRa Initialized");
  //--------- End Lora Settings

  // For gas sensor
  if ( Serial ) Serial.println("Initializing gas sensor");
  gas.begin(0x04);        //the default I2C address of the slave is 0x04
  gas.powerOn();
  if ( Serial ) Serial.println("Gas sensor Initialized");

  displayDebug("setup complete");

  txStatusActive();

  printMenu();
}

void loop() {  
  if (Serial) 
  {
    handleSerial();  //permit sending codes through serial
    Serial.println("Send a command to continue...");
    while(!Serial.available());
        delay(1000);
  }

    //broadcastData(AllValues);
}

void getData() {
  ValueNH3 = gas.measure_NH3();
  fullpayload[0] = round(ValueNH3 * 100);
    
  ValueCO = gas.measure_CO();
  fullpayload[1] = round(ValueCO * 100);

  ValueNO2 = gas.measure_NO2();
  fullpayload[2] = round(ValueNO2 * 100);

  ValueC3H8 = gas.measure_C3H8();
  fullpayload[3] = round(ValueC3H8 * 100);
  
  ValueC4H10 = gas.measure_C4H10();
  fullpayload[4] = round(ValueC4H10 * 100);

  ValueCH4 = gas.measure_CH4();
  fullpayload[5] = round(ValueCH4 * 100);

  ValueH2 = gas.measure_H2();
  fullpayload[6] = round(ValueH2 * 100);
  
  ValueC2H5OH = gas.measure_C2H5OH();
  fullpayload[7] = round(ValueC2H5OH * 100);

  if (transmitRequested == 1 )
  {
    broadcastData(fullpayload, sizeofFullPayload);
    transmitRequested = 0;
  }

}

void printData() {
  Serial.print(F("The concentration of Amonia (NH3) is "));
  if(ValueNH3>=0) Serial.print(ValueNH3);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println( String(fullpayload[0]) );
  //Amonia < 300ppm

  Serial.print(F("The concentration of carbon monoxide (CO) is "));
  if(ValueCO>=0) Serial.print(ValueCO);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println( String(fullpayload[1]) );
  // want < 70ppm

  Serial.print(F("The concentration of Nitrous dioxide (NO2) is "));
  if(ValueNO2>=0) Serial.print(ValueNO2);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println( String(fullpayload[2]) );
  //want < 5ppm

  Serial.print(F("The concentration of propane (C3H8) is "));
  if(ValueC3H8>=0) Serial.print(ValueC3H8);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println( String(fullpayload[3]) );
  //Propane < 2100PPM https://www.cdc.gov/niosh/idlh/74986.html (IDHL = Immediately Dangerous to Life or Health Concentrations)
  //More info on gases: https://safety.honeywell.com/content/dam/his-sandbox/products/gas-and-flame-detection/documents/Application-Note-202_The-ABC27s-Of-Gases-In-The-Industry_04-99.pdf

  Serial.print(F("The concentration of butane (C4H10) is "));
  if(ValueC4H10>=0) Serial.print(ValueC4H10);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println( String(fullpayload[4]) );
  //Butane < 1ppm STEL (short term exposure limit of < 15 minutes)

  Serial.print(F("The concentration of methane (CH4) is "));
  if(ValueCH4>=0) Serial.print(ValueCH4);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println( String(fullpayload[5]) );
  //methane no recommendations

  Serial.print(F("The concentration of hydrogen gas (H2) is "));
  if(ValueH2>=0) Serial.print(ValueH2);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println( String(fullpayload[6]) );
  //hydrogen no recommendations

  Serial.print(F("The concentration of ethyl alcohol (C2H5OH) is "));
  if(ValueC2H5OH>=0) Serial.print(ValueC2H5OH);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println( String(fullpayload[7]) );
  //ethyl alcohol < 3300 ppm
}
