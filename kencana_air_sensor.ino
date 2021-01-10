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
#define DEBUG 1
int outputLVL{3};                     //To enable debugging
bool debugPrinted {false};             //track if we've printed debug data (don't spam serial console)

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
byte TransPayLoad[32];                  // Byte array to store gas data for transmission https://www.thethingsnetwork.org/docs/devices/bytes.html
byte MessagePayload[2];               // Byte array for network messages
unsigned long previousWarnTx{0};
//uint32_t convertedValue;             // store data after reducing decimal places (by * 100) before high/low encoding into two bytes

//For timer
bool alarming {false};                 // track if we're currently sounding warning or alarm 
unsigned long previousMillis {0};               // stores the last time data collected
unsigned long setPointMillis;
int fiveMinCyclesCnt{0};
int TwentySecondCyclesCnt{0};
bool hourlyDataProcessed{0};
#define fourSeconds    4000            
#define twentySeconds  20000             
#define twoMinutes     120000                
#define fiveMinutes    300000                
#define fifteenMinutes 900000
#define oneHour        3600000

const int buzzerPin {10};
int beepCount {0};                                //number of times to beep()
bool silence {true};  //#DEBUG

//For gas sensor
//to pullup or not to pullup: https://forum.seeedstudio.com/t/problems-with-grove-multichannel-gas-sensor/6004/4
bool coAlarming {false};
bool propaneAlarming {false};
#define gasI2Caddress 4
byte gasI2Cerror = 9;                      //Track any I2C errors from gas sensor on startup, 9 = unrealistic number, just for init
unsigned char gasFirmwareversion;
int gasValueMapped;
float decodedValue;
int propaneMapped;                    //convert propane value to percent of STEL level. i.e. 1890 = 90% to 2100

struct gas_t {
  const int gas_id;
  float value;
  bool value_is_valid;
  const int minDetectable;
  const int maxDetectable;
  const int warn;
  const int alarm;
  float twentySecondObs[15];        //fifteen X 20s = five minutes of data
  float fiveMinAvgs[12];            //twelve X 5m = one hour of data
  float currentFiveMinAvg;
  float hourlyMin;
  float hourlyMax;
  float hourlyAvg;
};

//https://stackoverflow.com/questions/47883151/arduino-ide-does-not-allow-struct-variables-outside-a-function
gas_t gasNH3 = { 1, 0, 1, 1, 500, 200, 300, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasCO = { 2, 0, 1, 1, 1000, 50, 100, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; 
gas_t gasNO2 = { 3, 0, 1, 0.05, 10, 4, 5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasC3H8 = { 4, 0, 1, 0, 4000, 1500, 2100, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasC4H10 = { 5, 0, 1, 0, 1500, 900, 1000, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasCH4 = { 6, 0, 1, 0, 50000, 50000, 50000, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasH2 = { 7, 0, 1, 1, 1000, 1000, 1000, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasC2H5OH = { 8, 0, 1, 10, 500, 2000, 3300, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

void setup() {
  // Create a new NeoPixel object dynamically with these values:
  pixels = new Adafruit_NeoPixel(numPixels, NeoPin, pixelFormat);
  pixels->begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels->clear(); // Set all pixel colors to 'off'
  
  // configure buzzer pin as output
  pinMode(buzzerPin, OUTPUT); 

  #ifdef DEBUG 
    Serial.begin(115200);
    delay(2500);
    if ( Serial ) Serial.println("Serial enabled");
  #endif
  
  //--------- Lora Settings
  LoRa.setPins(RFM95_SS, RFM95_RST, RFM95_INT);
  #ifdef DEBUG
    if ( Serial ) Serial.println("Initializing LoRa");
  #endif
  while (!LoRa.begin(433E6)) {
    
   #ifdef DEBUG 
     if ( Serial ) Serial.print(".");
   #endif
    delay(500);
  }
  #ifdef DEBUG 
    if ( Serial ) Serial.println("LoRa Initialized");
  #endif
  //--------- End Lora Settings

  // For gas sensor
  Wire.begin();
  delay(1000);
  Wire.beginTransmission(gasI2Caddress);
  gasI2Cerror = Wire.endTransmission();  //#DEBUG need to utilize this more in loop()
  /* Wire errors: https://www.arduino.cc/en/Reference/WireEndTransmission
   * 0:success
   * 1:data too long to fit in transmit buffer
   * 2:received NACK on transmit of address
   * 3:received NACK on transmit of data
   * 4:other error
   */
  
  if (gasI2Cerror == 0)
  {
    #ifdef DEBUG 
      if ( Serial ) Serial.println("Initializing gas sensor");
    #endif
    gas.begin(gasI2Caddress);        //the default I2C address of the slave is 0x04
    gasFirmwareversion = gas.getVersion();
    gas.powerOn();
  }
  #ifndef DEBUG
    unsigned long currentMillis = millis();  //preheat sensor before we take readings
    while ( currentMillis < fifteenMinutes ) 
    {
      getData();
      delay(1000);
      splitYellow();
      currentMillis = millis();
    }
  #endif
  
  #ifdef DEBUG 
    else 
    {
      if ( Serial ) Serial.println("Gas sensor I2C address unresponsive.");
    }
    displayDebug();
  #endif

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
  if ( gasI2Cerror > 0 ) cycleRed();
  #ifdef DEBUG 
    if (Serial) handleSerial();
  #endif

  //Twenty-second routine
  unsigned long currentMillis = millis();
  unsigned long elapsedMillis = currentMillis - previousBlinked;
  if ( elapsedMillis > twentySeconds) 
  {
    getData();

    //#DEBUG this is quite cludgy—need to find a better way
    buildFiveMinuteData(gasNH3.value, gasNH3.twentySecondObs, gasNH3.currentFiveMinAvg, gasNH3.hourlyMin, gasNH3.hourlyMax );
    buildFiveMinuteData(gasCO.value, gasCO.twentySecondObs, gasCO.currentFiveMinAvg, gasCO.hourlyMin, gasCO.hourlyMax );
    buildFiveMinuteData(gasNO2.value, gasNO2.twentySecondObs, gasNO2.currentFiveMinAvg, gasNO2.hourlyMin, gasNO2.hourlyMax );
    buildFiveMinuteData(gasC3H8.value, gasC3H8.twentySecondObs, gasC3H8.currentFiveMinAvg, gasC3H8.hourlyMin, gasC3H8.hourlyMax );
    buildFiveMinuteData(gasC4H10.value, gasC4H10.twentySecondObs, gasC4H10.currentFiveMinAvg, gasC4H10.hourlyMin, gasC4H10.hourlyMax );
    buildFiveMinuteData(gasCH4.value, gasCH4.twentySecondObs, gasCH4.currentFiveMinAvg, gasCH4.hourlyMin, gasCH4.hourlyMax );
    buildFiveMinuteData(gasH2.value, gasH2.twentySecondObs, gasH2.currentFiveMinAvg, gasH2.hourlyMin, gasH2.hourlyMax );
    buildFiveMinuteData(gasC2H5OH.value, gasC2H5OH.twentySecondObs, gasC2H5OH.currentFiveMinAvg, gasC2H5OH.hourlyMin, gasC2H5OH.hourlyMax );

    //five-minute sub-routine
    if ( TwentySecondCyclesCnt == 15 ) {     
      gasNH3.fiveMinAvgs[fiveMinCyclesCnt] = gasNH3.currentFiveMinAvg;
      gasCO.fiveMinAvgs[fiveMinCyclesCnt] = gasCO.currentFiveMinAvg;
      gasNO2.fiveMinAvgs[fiveMinCyclesCnt] = gasNO2.currentFiveMinAvg;
      gasC3H8.fiveMinAvgs[fiveMinCyclesCnt] = gasC3H8.currentFiveMinAvg;
      gasC4H10.fiveMinAvgs[fiveMinCyclesCnt] = gasC4H10.currentFiveMinAvg;
      gasCH4.fiveMinAvgs[fiveMinCyclesCnt] = gasCH4.currentFiveMinAvg;
      gasH2.fiveMinAvgs[fiveMinCyclesCnt] = gasH2.currentFiveMinAvg;
      gasC2H5OH.fiveMinAvgs[fiveMinCyclesCnt] = gasC2H5OH.currentFiveMinAvg;
      TwentySecondCyclesCnt = 0;

      //five-minute sub-routine
      if ( fiveMinCyclesCnt == 12 ) {
        processHourlyData();
        fiveMinCyclesCnt = 0;
      }
      else fiveMinCyclesCnt++;
    }
    else TwentySecondCyclesCnt++;
    
    #ifdef DEBUG
      if ( outputLVL == 3 && Serial ) Serial.print("TwentySecondCyclesCnt: ") & Serial.println(TwentySecondCyclesCnt);
      if ( outputLVL == 3 && Serial ) Serial.print("fiveMinCyclesCnt: ") & Serial.println(fiveMinCyclesCnt); 
    #endif

    previousBlinked = millis();
    if ( alarming == false ) blinkGreen();
    blinked = true;
  }
  else blinked = false;

  //Five-minute routine
  currentMillis = millis();
  elapsedMillis = currentMillis - previousMillis;
  if ( elapsedMillis >= fiveMinutes )
  {
    previousMillis = currentMillis;
    if ( gasI2Cerror == 0 ) 
    {
      //getData(); 
      encodeData();
      //transmitData();
      broadcastData(webGatewayAddress, TransPayLoad);
    }
    #ifdef DEBUG
      if (Serial) printData();
    #endif
  }

  //Hourly routine
  if ( hourlyDataProcessed = true ) {
    hourlyDataProcessed = false;
    //encodeHourlyData
    //transmitHourlyData
    //resetHourlyData
  }

} //END loop()
void chirp()
{
  if ( silence == true ) return;
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
  if ( silence == true ) return;
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
  if ( silence == true ) return;
  for(int i=0; i<beepCount; i++)
  {
    tone(buzzerPin, 4000);
    delay(100);
    noTone(buzzerPin);
    delay(100);
    if ( i == beepCount ) break;
  }
}
