/*
 *Board: Adafruit Feather M0 RFM9x LoRa 433MHz
 *Sensor: https://wiki.seeedstudio.com/Grove-Multichannel_Gas_Sensor/
 *   based on: https://files.seeedstudio.com/wiki/Grove-Multichannel_Gas_Sensor/res/MiCS-6814_Datasheet.pdf
 *Might be a good read: https://learn.adafruit.com/adafruit-feather-m0-bluefruit-le/adapting-sketches-to-m0
 *Libraries MutichannelGasSensor.h and MutichannelGasSensor.cpp both adapted for Adafruit SAMD M0 boards (serial vs serialusb)
 *Info on gas sensors, set points, etc: http://media.clemson.edu/research/safety/GAS%20MONITORING%20PROGRAM.pdf
 *
 *Sensors detectable ranges:
 *
    Ammonia NH3           1 – 500ppm
    Carbon monoxide CO    1 – 1000ppm
    Nitrogen dioxide NO2  0.05 – 10ppm
    Propane C3H8          >1000ppm
    Iso-butane C4H10      >1000ppm
    Methane CH4           >1000ppm
    Hydrogen H2           1 – 1000ppm
    Ethanol C2H6OH        10 – 500ppm
*/

#include <Wire.h>                     //Needed for I2C 
#include "MutichannelGasSensor.h"     //Needed for gas sensor
#include <Adafruit_NeoPixel.h>

//For Neopixel
const int NeoPin {13};
const int numPixels {8};
const int pixelFormat = NEO_GRB + NEO_KHZ800;
Adafruit_NeoPixel *pixels;
bool blinked {false};
long previousBlinked {0};

//For LoRa 
#include <LoRa.h>                     // Needed for LoRa
const int RFM95_SS {8};               // The CS pin (#8) does not have a pullup built in so be sure to set this pin HIGH when not using the radio!
const int RFM95_RST {4};
const int RFM95_INT {3};
const byte localAddress {0x5};        // address of this device (0x5 is HEX "5")
const byte webGatewayAddress {0x1};   // address of web gateway
const byte broadcastAddress {0xFF};   // broadcast address
byte destination {0xFF};              // destination to send to
byte msgCount {0};                    // count of outgoing messages
bool expectingMessage {false};        // Indicates alarm sent, ack needed
bool messageReceived {false};         // Indicates a properly-formatted–but not necessarily appropriate–message received
String LastReceivedTrans;             // Record last message received by this station
String LastSentTrans;                 // Record last transmission sent from this station
byte GasPayLoad[32];                  // Byte array to store gas data for transmission https://www.thethingsnetwork.org/docs/devices/bytes.html
byte MessagePayload[2];               // Byte array for network messages
int sizeofGasPayLoad;
bool transmitRequested {0};
//uint32_t convertedValue;               // store data after reducing decimal places (by * 100) before high/low encoding into two bytes

//For timer
long previousMillis {0};                  // stores the last time data collected
unsigned long currentMillis; 
unsigned long setPointMillis;
const long fourSeconds {4000};            
const long twentySeconds {20000};               
const long twoMinutes {120000};                 
const long fiveMinutes {300000};                
const long fifteenMinutes {900000}; 

const int buzzerPin {10};
int beepCount {0};                                //number of times to beep()

//For gas sensor
//to pullup or not to pullup: https://forum.seeedstudio.com/t/problems-with-grove-multichannel-gas-sensor/6004/4
const byte gasI2Caddress = 4;
byte gasI2Cerror = 9;                      //Track any I2C errors from gas sensor on startup, 9 = (hopefully) unrealistic number, just for init
unsigned char gasFirmwareversion;
int gasValueMapped;
float decodedValue;
int propaneMapped;                    //convert propane value to percent of STEL level. i.e. 1890 = 90% to 2100
union gasUnion  //Used to convert float to bytes[4] adapted from: http://www.cplusplus.com/forum/beginner/18566/
{
        float gasVal;
        unsigned char gasBytes[4];
};
struct gas_t {
  const int gas_id;
  float value;
  bool value_is_valid;
  const int minDetectable;
  const int maxDetectable;
  const int warn;
  const int alarm;
};

//https://stackoverflow.com/questions/47883151/arduino-ide-does-not-allow-struct-variables-outside-a-function
gas_t gasNH3 = { 1, 0, 1, 1, 500, 200, 300 };
gas_t gasCO = { 2, 0, 1, 1, 1000, 50, 100 }; 
gas_t gasNO2 = { 3, 0, 1, 0.05, 10, 4, 5 };
gas_t gasC3H8 = { 4, 0, 1, 0, 4000, 1500, 2100 };
gas_t gasC4H10 = { 5, 0, 1, 0, 1500, 900, 1000 };
gas_t gasCH4 = { 6, 0, 1, 0, 50000, 50000, 50000 };
gas_t gasH2 = { 7, 0, 1, 1, 1000, 1000, 1000 };
gas_t gasC2H5OH = { 8, 0, 1, 10, 500, 2000, 3300 };

bool debug {true};                     //To enable debugging
bool debugPrinted {false};             //track if we've printed debug data (don't spam serial console)
String debugMessage;                   //optional message to add to displayDebug()

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
  Wire.begin();
  Wire.beginTransmission(gasI2Caddress);
  gasI2Cerror = Wire.endTransmission();
  
  if (gasI2Cerror == 0)
  {
    if ( Serial ) Serial.println("Initializing gas sensor");
    gas.begin(gasI2Caddress);        //the default I2C address of the slave is 0x04
    gasFirmwareversion = gas.getVersion();
    gas.powerOn();
  if ( Serial ) Serial.println("Gas sensor Initialized");
  }
  else 
  {
    if ( Serial ) Serial.println("Gas sensor I2C address unresponsive.");
  }

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

  splitBlue();
}

void loop() 
{
  if (Serial) handleSerial();  //permit sending codes through serial
  currentMillis = millis();
  if ( currentMillis - previousBlinked > twentySeconds ) 
  {
    getData();
    previousBlinked = currentMillis;
    blinkGreen();
    blinked = true;
  }
  else blinked = false;
  if ( currentMillis - previousMillis > fiveMinutes )
  {
    previousMillis = currentMillis;
    if ( gasI2Cerror == 0 ) 
    {
      getData(); 
      encodeData();
      transmitData();
    }
    if (Serial) printData();
  }

  //Carbon Monoxide warn
  if ( gasI2Cerror == 0 && (gasCO.value >= gasCO.warn && gasCO.value < gasCO.alarm) )
  {
    getData();
    beep(5);
    splitYellow();
  }
  //Propane warn
  if ( gasI2Cerror == 0 && (gasC3H8.value >= gasC3H8.warn && gasC3H8.value < gasC3H8.alarm) )
  {
    getData(); 
    propaneMapped = map(gasC3H8.value,0,2100,0,100);        //convert propane value to percent of STEL level. i.e. 1890 = 90% to 2100
    propaneMapped = constrain(propaneMapped, 0, 100);   //constrain possible values to range of 0 - 100
    neoPercent(propaneMapped);  
  }
  
  /*// alarm and frequent checks
  if ( gasI2Cerror == 0 && (gasCO.value >= 70 || gasC3H8.value >= 2100 || gasC4H10.value >= 1000) )
  {
    gasCO.value = gas.measure_CO();
    gasC3H8.value = gas.measure_C3H8();
    gasC4H10.value = gas.measure_C4H10();
    alarmRed();
  }*/
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

void beep(int beepCount)
{
  for(int i=0; i<beepCount; i++)
  {
    tone(buzzerPin, 4000);
    delay(100);
    noTone(buzzerPin);
    delay(100);
    if ( i == beepCount ) break;
  }
}
