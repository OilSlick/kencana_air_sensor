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
byte localAddress = 0x5;              // address of this device (0x5 is HEX "5")
byte webGatewayAddress = 0x1;         // address of web gateway
byte broadcastAddress = 0xFF;         // broadcast address
byte destination = 0xFF;              // destination to send to
byte msgCount = 0;                    // count of outgoing messages
bool expectingMessage = false;        //Indicates alarm sent, ack needed
bool messageReceived = false;         //Indicates a properly-formatted–but not necessarily appropriate–message received
String outgoingMsg;                   //contents of outgoing radio transmission
String LastReceivedTrans;             //Record last message received by this station
String LastSentTrans;                 //Record last transmission sent from this station
byte fullpayload[16];                  //Byte array to store data for transmission https://www.thethingsnetwork.org/docs/devices/bytes.html
int sizeofFullPayload;
bool transmitRequested = 0;
uint32_t convertedValue;               // store data after reducing decimal places (by * 100) before high/low encoding into two bytes

//For timer
long previousMillis = 0;                  // stores the last time data collected
unsigned long currentMillis;              // Used for crude timmer
long fiveMinutes = 300000;                // Polling interval in minutes * 60 * 1000
long fifteenMinutes = 900000;             // Polling interval in minutes * 60 * 1000

#define buzzerPin 10

//For gas sensor
float decodedValue;
float ValueNH3;
float ValueCO;
float ValueNO2;
float ValueC3H8;
float ValueC4H10;
float ValueCH4;
float ValueH2;
float ValueC2H5OH;

bool debug = true;                     //To enable debugging
bool debugPrinted = false;             //track if we've printed debug data (don't spam serial console)
String debugMessage;                   //optional message to add to displayDebug()

//For home alarm network
bool suppressAlarm = false;            //When 'true' an active alarm is silenced
bool alarmState = false;               //Determines if audible alarm has been activated
bool ReceivedAlarmStatus = 0;          //Track local unit alarm status (always byte[0])
byte ReceivedPayload;                  //Track byte[1]
bool alarmReceived = false;            //If another sensor sends alarm, this sensor is aware
bool nightMode = false;                //Nightmode = different alarm (persistent and annoying)
bool awayMode = false;                 //awayMode = activated by NFC card @ door

void setup() {
  // configure buzzer pin as output
  pinMode(buzzerPin, OUTPUT); 
  
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

  txStatusOnline();

  tone(buzzerPin, 2000); // Send tone
  delay(50);        
  noTone(buzzerPin);     // Stop sound...
  delay(50);
  tone(buzzerPin, 3000); // Send tone
  delay(50);        
  noTone(buzzerPin);     // Stop sound...
  delay(50);
  tone(buzzerPin, 4000); // Send tone
  delay(50);        
  noTone(buzzerPin);     // Stop sound...
  delay(1000);
}

void loop() 
{  
  currentMillis = millis();
  if ( currentMillis - previousMillis > fiveMinutes )
  {
    previousMillis = currentMillis;
    transmitRequested = 1;
    getData();
    if (Serial) printData();
  }
  if (Serial) 
  {
    handleSerial();  //permit sending codes through serial
    //while(!Serial.available());
  }

}

void getData() {
  ValueNH3 = gas.measure_NH3();
  convertedValue = ValueNH3 * 100;
  fullpayload[0] = highByte(convertedValue);
  fullpayload[1] = lowByte(convertedValue);
    
  ValueCO = gas.measure_CO();
  convertedValue = ValueCO * 100;
  fullpayload[2] = highByte(convertedValue);
  fullpayload[3] = lowByte(convertedValue);

  ValueNO2 = gas.measure_NO2();
  convertedValue = ValueNO2 * 100;
  fullpayload[4] = highByte(convertedValue);
  fullpayload[5] = lowByte(convertedValue);

  ValueC3H8 = gas.measure_C3H8();
  convertedValue = ValueC3H8 * 100;
  fullpayload[6] = highByte(convertedValue);
  fullpayload[7] = lowByte(convertedValue);
  
  ValueC4H10 = gas.measure_C4H10();
  convertedValue = ValueC4H10 * 100;
  fullpayload[8] = highByte(convertedValue);
  fullpayload[9] = lowByte(convertedValue);

  ValueCH4 = gas.measure_CH4();
  convertedValue = ValueCH4 * 100;
  fullpayload[10] = highByte(convertedValue);
  fullpayload[11] = lowByte(convertedValue);

  ValueH2 = gas.measure_H2();
  convertedValue = ValueH2 * 100;
  fullpayload[12] = highByte(convertedValue);
  fullpayload[13] = lowByte(convertedValue);
  
  ValueC2H5OH = gas.measure_C2H5OH();
  convertedValue = ValueC2H5OH * 100;
  fullpayload[14] = highByte(convertedValue);
  fullpayload[15] = lowByte(convertedValue);

  if (transmitRequested == 1 )
  {
    destination = webGatewayAddress;
    broadcastData(destination, fullpayload, sizeof(fullpayload));
    transmitRequested = 0;
  }

}

void printData() {
  Serial.print(F("Amonia (NH3): "));
  decodedValue = (fullpayload[0] << 8) + fullpayload[1];
  ValueNH3 = decodedValue /100;
  if(ValueNH3>=0) Serial.print(ValueNH3);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if(ValueNH3>=200) Serial.print(" (value high)");
  //Serial.println( String(fullpayload[0]) ); //#DEBUG
  Serial.println();
  //Amonia < 300ppm

  decodedValue = (fullpayload[2] << 8) + fullpayload[3];
  ValueCO = decodedValue /100;
  Serial.print(F("carbon monoxide (CO): "));
  if(ValueCO>=0) Serial.print(ValueCO);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if(ValueCO>=50) 
  {
    chirp();
    Serial.print(" (value high)");
  }
  //Serial.println( String(fullpayload[1]) ); //#DEBUG
  Serial.println();
  // want < 70ppm https://www.cacgas.com.au/blog/carbon-monoxide-co-toxic-gas-workplace-safety

  decodedValue = (fullpayload[4] << 8) + fullpayload[5];
  ValueNO2 = decodedValue /100;
  Serial.print(F("Nitrous dioxide (NO2): "));
  if(ValueNO2>=0) Serial.print(ValueNO2);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if(ValueNO2>=4) Serial.print(" (value high)");
  //Serial.println( String(fullpayload[2]) ); //#DEBUG
  Serial.println();
  //want < 5ppm

  decodedValue = (fullpayload[6] << 8) + fullpayload[7];
  ValueC3H8 = decodedValue /100;
  Serial.print(F("Propane (C3H8): "));
  if(ValueC3H8>=0) Serial.print(ValueC3H8);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if(ValueC3H8>=1500) Serial.print(" (value high)");
  //Serial.println( String(fullpayload[3]) ); //#DEBUG
  Serial.println();
  //Propane < 2100PPM https://www.cdc.gov/niosh/idlh/74986.html (IDHL = Immediately Dangerous to Life or Health Concentrations)
  //More info on gases: https://safety.honeywell.com/content/dam/his-sandbox/products/gas-and-flame-detection/documents/Application-Note-202_The-ABC27s-Of-Gases-In-The-Industry_04-99.pdf

  decodedValue = (fullpayload[8] << 8) + fullpayload[9];
  ValueC4H10 = decodedValue /100;
  Serial.print(F("Butane (C4H10): "));
  if(ValueC4H10>=0) Serial.print(ValueC4H10);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if(ValueC4H10>=900) Serial.print(" (value high)");
  //Serial.println( String(fullpayload[4]) ); //#DEBUG
  Serial.println();
  //Butane < 1000ppm STEL (short term exposure limit of < 15 minutes) https://pubchem.ncbi.nlm.nih.gov/compound/Butane#section=Immediately-Dangerous-to-Life-or-Health-(IDLH)

  decodedValue = (fullpayload[10] << 8) + fullpayload[11];
  ValueCH4 = decodedValue /100;
  Serial.print(F("Methane (CH4): "));
  if(ValueCH4>=0) Serial.print(ValueCH4);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  //Serial.println( String(fullpayload[5]) ); //#DEBUG
  Serial.println();
  //methane no recommendations

  decodedValue = (fullpayload[12] << 8) + fullpayload[13];
  ValueH2 = decodedValue /100;
  Serial.print(F("Hydrogen gas (H2): "));
  if(ValueH2>=0) Serial.print(ValueH2);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  //Serial.println( String(fullpayload[6]) ); //#DEBUG
  Serial.println();
  //hydrogen no recommendations

  decodedValue = (fullpayload[14] << 8) + fullpayload[15];
  ValueC2H5OH = decodedValue /100;
  Serial.print(F("Ethyl alcohol (C2H5OH): "));
  if(ValueC2H5OH>=0) Serial.print(ValueC2H5OH);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if(ValueC2H5OH>=2500) Serial.print(" (value high)");
  //Serial.println( String(fullpayload[7]) ); //#DEBUG
  Serial.println();
  //ethyl alcohol < 3300 ppm
  Serial.println();
  Serial.println();
  Serial.println();
}
void soundAlarm()
{
  while ( suppressAlarm == false )
  {
    chirp();
    //checkUnackedAlarm();
    //checkMessages();
    onLoRaReceive(LoRa.parsePacket());  //if LoRa packet received, parse it
  }
  alarmState = false;
}
void chirp()
{
  tone(buzzerPin, 4000); // Send 1KHz sound signal...
  delay(100);        
  noTone(buzzerPin);     // Stop sound...
  delay(100);
  tone(buzzerPin, 4000); // Send 1KHz sound signal...
  delay(100);        
  noTone(buzzerPin);     // Stop sound...
  delay(100); 
}
