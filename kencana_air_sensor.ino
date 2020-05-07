/*
 *Board: Adafruit Feather M0 RFM9x LoRa 433MHz
 *Sensor: https://wiki.seeedstudio.com/Grove-Multichannel_Gas_Sensor/
 *   based on: https://files.seeedstudio.com/wiki/Grove-Multichannel_Gas_Sensor/res/MiCS-6814_Datasheet.pdf
 *Might be a good read: https://learn.adafruit.com/adafruit-feather-m0-bluefruit-le/adapting-sketches-to-m0
 *Libraries MutichannelGasSensor.h and MutichannelGasSensor.cpp both adapted for Adafruit SAMD M0 boards (serial vs serialusb)
 *
 *Sensor detectable ranges:
 *
    Carbon monoxide CO 1 – 1000ppm
    Nitrogen dioxide NO2 0.05 – 10ppm
    Ethanol C2H6OH 10 – 500ppm
    Hydrogen H2 1 – 1000ppm
    Ammonia NH3 1 – 500ppm
    Methane CH4 >1000ppm
    Propane C3H8 >1000ppm
    Iso-butane C4H10 >1000ppm

*/

#include <Wire.h>                     //Needed for I2C 
#include "MutichannelGasSensor.h"     //Needed for gas sensor
#include <Adafruit_NeoPixel.h>

//For Neopixel
int NeoPin = 13;
int numPixels = 8;
int pixelFormat = NEO_GRB + NEO_KHZ800;
Adafruit_NeoPixel *pixels;

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
byte GasPayLoad[16];                  //Byte array to store gas data for transmission https://www.thethingsnetwork.org/docs/devices/bytes.html
byte MessagePayload[2];               //Byte array for network messages
int sizeofGasPayLoad;
bool transmitRequested = 0;
uint32_t convertedValue;               // store data after reducing decimal places (by * 100) before high/low encoding into two bytes

//For timer
long previousMillis = 0;                  // stores the last time data collected
unsigned long currentMillis;              // Used for crude timmer
long twoMinutes = 120000;                 // Polling interval in minutes * 60 * 1000
long fiveMinutes = 300000;                // Polling interval in minutes * 60 * 1000
long fifteenMinutes = 900000;             // Polling interval in minutes * 60 * 1000

#define buzzerPin 10

//For gas sensor
//to pullup or not to pullup: https://forum.seeedstudio.com/t/problems-with-grove-multichannel-gas-sensor/6004/4
unsigned char gasFirmwareversion;
int gasValueMapped;
float decodedValue;
float ValueNH3;
float ValueCO;
float ValueNO2;
float ValueC3H8;
int propaneMapped;                     //convert propane value to percent of STEL level. i.e. 1890 = 90% to 2100
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
  // Create a new NeoPixel object dynamically with these values:
  pixels = new Adafruit_NeoPixel(numPixels, NeoPin, pixelFormat);
  pixels->begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels->clear(); // Set all pixel colors to 'off'
  
  // configure buzzer pin as output
  pinMode(buzzerPin, OUTPUT); 
  
  Serial.begin(115200);
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
  gasFirmwareversion = gas.getVersion();
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

  cycleGreen();
}

void loop() 
{
  /*if ( nightMode == true && alarmReceived == true )  //if another sensor has sounded an alarm, lets make noise too!
  {
    soundAlarm();
  }
  else if ( alarmReceived == false && alarmState == false && suppressAlarm == true )
  {
    suppressAlarm = false; //reset suppressAlarm
  } */
  if (Serial) handleSerial();  //permit sending codes through serial
  currentMillis = millis();
  if ( currentMillis - previousMillis > fiveMinutes )
  {
    previousMillis = currentMillis;
    transmitRequested = 1;
    getData();
    if (Serial) printData();
  }
  // warning flash and frequent checks
  if ( ValueCO >= 50 || ValueC3H8 >= 100 || ValueC4H10 >= 850 )
  {
    ValueCO = gas.measure_CO();
    ValueC3H8 = gas.measure_C3H8();
    propaneMapped = map(ValueC3H8,0,1100,0,100);    //convert propane value to percent of STEL level. i.e. 1890 = 90% to 2100
    neoPercent(propaneMapped);
    ValueC4H10 = gas.measure_C4H10();
    chirp();
    pixels->setPixelColor(0, pixels->Color(70, 70, 0));
    pixels->show();   // Send the updated pixel colors to the hardware.
    delay(100);
    pixels->setPixelColor(0, pixels->Color(0, 0, 0));
    pixels->show();
    delay(100);
    pixels->setPixelColor(0, pixels->Color(70, 70, 0));
    pixels->show();   // Send the updated pixel colors to the hardware.
    delay(100);
    pixels->setPixelColor(0, pixels->Color(0, 0, 0));
    pixels->show();
    delay(300);
  }
  // alarm and frequent checks
  if ( ValueCO >= 70 || ValueC3H8 >= 2100 || ValueC4H10 >= 1000 )
  {
    ValueCO = gas.measure_CO();
    ValueC3H8 = gas.measure_C3H8();
    ValueC4H10 = gas.measure_C4H10();
    alarmRed();
  }
  onLoRaReceive(LoRa.parsePacket());  //if LoRa packet received, parse it
}
void soundAlarm()
{
  while ( suppressAlarm == false )
  {
    chirp();
    onLoRaReceive(LoRa.parsePacket());  //if LoRa packet received, parse it
  }
  alarmState = false;
}
void chirp()
{
  tone(buzzerPin, 2000); // Send sound signal...
  delay(300);        
  noTone(buzzerPin);     // Stop sound...
  delay(100);
  tone(buzzerPin, 4000); // Send sound signal...
  delay(100);        
  noTone(buzzerPin);     // Stop sound...
  delay(100); 
}
void alarm()
{
  chirp();
  cycleRed();
  pixels->clear();
  chirp();
  cycleRed();
  pixels->clear();
  chirp();
  cycleRed();
  pixels->clear();
  delay(1000);
  chirp();
  chirp();
  chirp();
}
