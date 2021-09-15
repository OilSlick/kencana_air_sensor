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
#ifdef DEBUG
  bool NOTX{1};                         //1 = no transmission, radio silence
  int outputLVL{3};                     //level of verboseness
  bool debugPrinted {false};            //track if we've printed debug data (don't spam serial console)
#endif

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
byte TransPayLoad[32]{0};                  // Byte array to store gas data for transmission https://www.thethingsnetwork.org/docs/devices/bytes.html
unsigned long previousWarnTx{0};

//For timer
bool alarming {false};                 // track if we're currently sounding warning or alarm 
unsigned long previousMillis{0};               // stores the last time data collected
unsigned long setPointMillis{0};
int fiveMinCyclesCnt{0};
int TwentySecondCyclesCnt{0};
bool hourlyDataProcessed{0};      
#define twentySeconds  20000                 
#define fiveMinutes    300000                
#define fifteenMinutes 900000
#define oneHour        3600000

const int buzzerPin {10};
int beepCount {0};                                //number of times to beep()
bool silence {false};

//For gas sensor
//to pullup or not to pullup: https://forum.seeedstudio.com/t/problems-with-grove-multichannel-gas-sensor/6004/4
#define gasI2Caddress 4
byte gasI2Cerror = 9;                      //Track any I2C errors from gas sensor on startup, 9 = unrealistic number, just for init
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
gas_t gasNH3 = { 1, 0, 1, 1000, 500000, 200, 300, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasCO = { 2, 0, 1, 1000, 1000000, 50, 100, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; 
gas_t gasNO2 = { 3, 0, 1, 50, 10000, 4, 5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasC3H8 = { 4, 0, 1, 0, 4000000, 1500, 2100, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasC4H10 = { 5, 0, 1, 0, 1500000, 900, 1000, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasCH4 = { 6, 0, 1, 0, 50000000, 50000, 50000, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasH2 = { 7, 0, 1, 1000, 1000000, 1000, 1000, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
gas_t gasC2H5OH = { 8, 0, 1, 10000, 500000, 2000, 3300, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

void setup() {
  #ifdef DEBUG
    silence = true;
  #endif
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
    if (gasI2Cerror != 0) return;

    gasNH3.twentySecondObs[TwentySecondCyclesCnt] = gasNH3.value;
    gasCO.twentySecondObs[TwentySecondCyclesCnt] = gasCO.value;
    gasNO2.twentySecondObs[TwentySecondCyclesCnt] = gasNO2.value;
    gasC3H8.twentySecondObs[TwentySecondCyclesCnt] = gasC3H8.value;
    gasC4H10.twentySecondObs[TwentySecondCyclesCnt] = gasC4H10.value;
    gasCH4.twentySecondObs[TwentySecondCyclesCnt] = gasCH4.value;
    gasH2.twentySecondObs[TwentySecondCyclesCnt] = gasH2.value;
    gasC2H5OH.twentySecondObs[TwentySecondCyclesCnt] = gasC2H5OH.value;

    if ( TwentySecondCyclesCnt == 14 ) { 
      gasNH3.currentFiveMinAvg = gasNH3.fiveMinAvgs[fiveMinCyclesCnt] = buildFiveMinuteData(gasNH3.twentySecondObs );
      if ( fiveMinCyclesCnt == 0 ) {
        gasNH3.hourlyMin = gasNH3.fiveMinAvgs[fiveMinCyclesCnt];
        gasNH3.hourlyMax = gasNH3.fiveMinAvgs[fiveMinCyclesCnt];
      }
      else {
        if ( gasNH3.fiveMinAvgs[fiveMinCyclesCnt] < gasNH3.hourlyMin ) gasNH3.hourlyMin = gasNH3.fiveMinAvgs[fiveMinCyclesCnt];
        if ( gasNH3.fiveMinAvgs[fiveMinCyclesCnt] < gasNH3.hourlyMax ) gasNH3.hourlyMax = gasNH3.fiveMinAvgs[fiveMinCyclesCnt];
      }
      
      gasCO.currentFiveMinAvg = gasCO.fiveMinAvgs[fiveMinCyclesCnt] = buildFiveMinuteData(gasCO.twentySecondObs );
      if ( fiveMinCyclesCnt == 0 ) {
        gasCO.hourlyMin = gasCO.fiveMinAvgs[fiveMinCyclesCnt];
        gasCO.hourlyMax = gasCO.fiveMinAvgs[fiveMinCyclesCnt];
      }
      else {
        if ( gasCO.fiveMinAvgs[fiveMinCyclesCnt] < gasCO.hourlyMin ) gasCO.hourlyMin = gasCO.fiveMinAvgs[fiveMinCyclesCnt];
        if ( gasCO.fiveMinAvgs[fiveMinCyclesCnt] < gasCO.hourlyMax ) gasCO.hourlyMax = gasCO.fiveMinAvgs[fiveMinCyclesCnt];
      }
      
      gasNO2.currentFiveMinAvg = gasNO2.fiveMinAvgs[fiveMinCyclesCnt] = buildFiveMinuteData(gasNO2.twentySecondObs );
      if ( fiveMinCyclesCnt == 0 ) {
        gasNO2.hourlyMin = gasNO2.fiveMinAvgs[fiveMinCyclesCnt];
        gasNO2.hourlyMax = gasNO2.fiveMinAvgs[fiveMinCyclesCnt];
      }
      else {
        if ( gasNO2.fiveMinAvgs[fiveMinCyclesCnt] < gasNO2.hourlyMin ) gasNO2.hourlyMin = gasNO2.fiveMinAvgs[fiveMinCyclesCnt];
        if ( gasNO2.fiveMinAvgs[fiveMinCyclesCnt] < gasNO2.hourlyMax ) gasNO2.hourlyMax = gasNO2.fiveMinAvgs[fiveMinCyclesCnt];
      }
      
      gasC3H8.currentFiveMinAvg = gasC3H8.fiveMinAvgs[fiveMinCyclesCnt] = buildFiveMinuteData(gasC3H8.twentySecondObs );
      if ( fiveMinCyclesCnt == 0 ) {
        gasC3H8.hourlyMin = gasC3H8.fiveMinAvgs[fiveMinCyclesCnt];
        gasC3H8.hourlyMax = gasC3H8.fiveMinAvgs[fiveMinCyclesCnt];
      }
      else {
        if ( gasC3H8.fiveMinAvgs[fiveMinCyclesCnt] < gasC3H8.hourlyMin ) gasC3H8.hourlyMin = gasC3H8.fiveMinAvgs[fiveMinCyclesCnt];
        if ( gasC3H8.fiveMinAvgs[fiveMinCyclesCnt] < gasC3H8.hourlyMax ) gasC3H8.hourlyMax = gasC3H8.fiveMinAvgs[fiveMinCyclesCnt];
      }
      
      gasC4H10.currentFiveMinAvg = gasC4H10.fiveMinAvgs[fiveMinCyclesCnt] = buildFiveMinuteData(gasC4H10.twentySecondObs );
      if ( fiveMinCyclesCnt == 0 ) {
        gasC4H10.hourlyMin = gasC4H10.fiveMinAvgs[fiveMinCyclesCnt];
        gasC4H10.hourlyMax = gasC4H10.fiveMinAvgs[fiveMinCyclesCnt];
      }
      else {
        if ( gasC4H10.fiveMinAvgs[fiveMinCyclesCnt] < gasC4H10.hourlyMin ) gasC4H10.hourlyMin = gasC4H10.fiveMinAvgs[fiveMinCyclesCnt];
        if ( gasC4H10.fiveMinAvgs[fiveMinCyclesCnt] < gasC4H10.hourlyMax ) gasC4H10.hourlyMax = gasC4H10.fiveMinAvgs[fiveMinCyclesCnt];
      }
      
      gasCH4.currentFiveMinAvg = gasCH4.fiveMinAvgs[fiveMinCyclesCnt] = buildFiveMinuteData(gasCH4.twentySecondObs );
      if ( fiveMinCyclesCnt == 0 ) {
        gasCH4.hourlyMin = gasCH4.fiveMinAvgs[fiveMinCyclesCnt];
        gasCH4.hourlyMax = gasCH4.fiveMinAvgs[fiveMinCyclesCnt];
      }
      else {
        if ( gasCH4.fiveMinAvgs[fiveMinCyclesCnt] < gasCH4.hourlyMin ) gasCH4.hourlyMin = gasCH4.fiveMinAvgs[fiveMinCyclesCnt];
        if ( gasCH4.fiveMinAvgs[fiveMinCyclesCnt] < gasCH4.hourlyMax ) gasCH4.hourlyMax = gasCH4.fiveMinAvgs[fiveMinCyclesCnt];
      }
      
      gasH2.currentFiveMinAvg = gasH2.fiveMinAvgs[fiveMinCyclesCnt] = buildFiveMinuteData(gasH2.twentySecondObs );
      if ( fiveMinCyclesCnt == 0 ) {
        gasH2.hourlyMin = gasH2.fiveMinAvgs[fiveMinCyclesCnt];
        gasH2.hourlyMax = gasH2.fiveMinAvgs[fiveMinCyclesCnt];
      }
      else {
        if ( gasH2.fiveMinAvgs[fiveMinCyclesCnt] < gasH2.hourlyMin ) gasH2.hourlyMin = gasH2.fiveMinAvgs[fiveMinCyclesCnt];
        if ( gasH2.fiveMinAvgs[fiveMinCyclesCnt] < gasH2.hourlyMax ) gasH2.hourlyMax = gasH2.fiveMinAvgs[fiveMinCyclesCnt];
      }
     
      gasC2H5OH.currentFiveMinAvg = gasC2H5OH.fiveMinAvgs[fiveMinCyclesCnt] = buildFiveMinuteData(gasC2H5OH.twentySecondObs );
      if ( fiveMinCyclesCnt == 0 ) {
        gasC2H5OH.hourlyMin = gasC2H5OH.fiveMinAvgs[fiveMinCyclesCnt];
        gasC2H5OH.hourlyMax = gasC2H5OH.fiveMinAvgs[fiveMinCyclesCnt];
      }
      else {
        if ( gasC2H5OH.fiveMinAvgs[fiveMinCyclesCnt] < gasC2H5OH.hourlyMin ) gasC2H5OH.hourlyMin = gasC2H5OH.fiveMinAvgs[fiveMinCyclesCnt];
        if ( gasC2H5OH.fiveMinAvgs[fiveMinCyclesCnt] < gasC2H5OH.hourlyMax ) gasC2H5OH.hourlyMax = gasC2H5OH.fiveMinAvgs[fiveMinCyclesCnt];
      }
      if ( gasI2Cerror == 0 ) 
      {
        encodeData();
        broadcastData(webGatewayAddress, TransPayLoad, sizeof(TransPayLoad));
        #ifdef DEBUG
          if (Serial) displayCurrentFiveMin();
        #endif
        gasNH3.currentFiveMinAvg = 0;       //five-minute reset
        gasCO.currentFiveMinAvg = 0;
        gasNO2.currentFiveMinAvg = 0;
        gasC3H8.currentFiveMinAvg = 0;
        gasC4H10.currentFiveMinAvg = 0;
        gasCH4.currentFiveMinAvg = 0;
        gasH2.currentFiveMinAvg = 0;
        gasC2H5OH.currentFiveMinAvg = 0;
      }

      //// Hourly Routine ////
      if ( fiveMinCyclesCnt == 11 ) {   
        processHourlyData();
        fiveMinCyclesCnt = 0;
      }
      else fiveMinCyclesCnt++;
      TwentySecondCyclesCnt = 0;
    }
    else TwentySecondCyclesCnt++;
    
    #ifdef DEBUG
      displayCounts();
    #endif

    previousBlinked = millis();
    if ( alarming == false ) blinkGreen();
    blinked = true;
  }
  else blinked = false;

  //Hourly routine
  if ( hourlyDataProcessed == true ) {
    #ifdef DEBUG
      displayHourlyStats();
    #endif
    hourlyDataProcessed = false;
    encode_sendHourlyData();
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
