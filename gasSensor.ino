void getData() {
  if ( gasI2Cerror == 0 ) 
  {
    //convert all to ppb from ppm 
    gasNH3.value = gas.measure_NH3() * 1000;
    gasCO.value = gas.measure_CO() * 1000;
    gasNO2.value = gas.measure_NO2() * 1000;
    gasC3H8.value = gas.measure_C3H8() * 1000;
    gasC4H10.value = gas.measure_C4H10() * 1000;
    gasCH4.value = gas.measure_CH4() * 1000;
    gasH2.value = gas.measure_H2() * 1000;
    gasC2H5OH.value = gas.measure_C2H5OH() * 1000; 
  }
}
float buildFiveMinuteData(float (&twentySecondgas)[15]) {
  float currentFiveMinAvg{0};
  float FiveMinSum{0};
  for (int i = 0; i < TwentySecondCyclesCnt; i++) {
    FiveMinSum = FiveMinSum + twentySecondgas[i];
    if ( i == TwentySecondCyclesCnt ) break;
  }
  currentFiveMinAvg = FiveMinSum / ( TwentySecondCyclesCnt + 1 );  //have to add 1 because the first obs is "0", second is "1", etc...
  return currentFiveMinAvg;
}
void encodeData() {
  union gasUnion  //Used to convert float to bytes[4] adapted from: http://www.cplusplus.com/forum/beginner/18566/
  {
    float gasFloat;
    unsigned char gasBytes[4];
  };
  gasUnion a;  
   
  a.gasFloat = gasNH3.currentFiveMinAvg;
  TransPayLoad[0] = a.gasBytes[0];
  TransPayLoad[1] = a.gasBytes[1];
  TransPayLoad[2] = a.gasBytes[2];
  TransPayLoad[3] = a.gasBytes[3];
  
  a.gasFloat = gasCO.currentFiveMinAvg;
  TransPayLoad[4] = a.gasBytes[0];
  TransPayLoad[5] = a.gasBytes[1];
  TransPayLoad[6] = a.gasBytes[2];
  TransPayLoad[7] = a.gasBytes[3];
   
  a.gasFloat = gasNO2.currentFiveMinAvg;
  TransPayLoad[8] = a.gasBytes[0];
  TransPayLoad[9] = a.gasBytes[1];
  TransPayLoad[10] = a.gasBytes[2];
  TransPayLoad[11] = a.gasBytes[3];
  
  a.gasFloat = gasC3H8.currentFiveMinAvg;
  TransPayLoad[12] = a.gasBytes[0];
  TransPayLoad[13] = a.gasBytes[1];
  TransPayLoad[14] = a.gasBytes[2];
  TransPayLoad[15] = a.gasBytes[3];
  
  a.gasFloat = gasC4H10.currentFiveMinAvg;
  TransPayLoad[16] = a.gasBytes[0];
  TransPayLoad[17] = a.gasBytes[1];
  TransPayLoad[18] = a.gasBytes[2];
  TransPayLoad[19] = a.gasBytes[3];

  a.gasFloat = gasCH4.currentFiveMinAvg;
  TransPayLoad[20] = a.gasBytes[0];
  TransPayLoad[21] = a.gasBytes[1];
  TransPayLoad[22] = a.gasBytes[2];
  TransPayLoad[23] = a.gasBytes[3];
  
  a.gasFloat = gasH2.currentFiveMinAvg;
  TransPayLoad[24] = a.gasBytes[0];
  TransPayLoad[25] = a.gasBytes[1];
  TransPayLoad[26] = a.gasBytes[2];
  TransPayLoad[27] = a.gasBytes[3];
  
  a.gasFloat = gasC2H5OH.currentFiveMinAvg;
  TransPayLoad[28] = a.gasBytes[0];
  TransPayLoad[29] = a.gasBytes[1];
  TransPayLoad[30] = a.gasBytes[2];
  TransPayLoad[31] = a.gasBytes[3];  
}
void processHourlyData() {
  float gasNH3hourlySum{0.0};
  float gasCOhourlySum{0.0};
  float gasNO2hourlySum{0.0};
  float gasC3H8hourlySum{0.0};
  float gasC4H10hourlySum{0.0};
  float gasCH4hourlySum{0.0};
  float gasH2hourlySum{0.0};
  float gasC2H5OHhourlySum{0.0};
  for ( int i = 0; i < 11; i++ ) {
    gasNH3hourlySum += gasNH3.fiveMinAvgs[i];
    gasCOhourlySum += gasCO.fiveMinAvgs[i];
    gasNO2hourlySum += gasNO2.fiveMinAvgs[i];
    gasC3H8hourlySum += gasC3H8.fiveMinAvgs[i];
    gasC4H10hourlySum += gasC4H10.fiveMinAvgs[i];
    gasCH4hourlySum += gasCH4.fiveMinAvgs[i];
    gasH2hourlySum += gasH2.fiveMinAvgs[i];
    gasC2H5OHhourlySum += gasC2H5OH.fiveMinAvgs[i];
  }
  gasNH3.hourlyAvg = gasNH3hourlySum / 12;
  gasCO.hourlyAvg = gasCOhourlySum / 12;
  gasNO2.hourlyAvg = gasNO2hourlySum / 12;
  gasC3H8.hourlyAvg = gasC3H8hourlySum / 12;
  gasC4H10.hourlyAvg = gasC4H10hourlySum / 12;
  gasCH4.hourlyAvg = gasCH4hourlySum / 12;
  gasH2.hourlyAvg = gasH2hourlySum / 12;
  gasC2H5OH.hourlyAvg = gasC2H5OHhourlySum / 12;
    
  hourlyDataProcessed = true;
  #ifdef DEBUG
  if (outputLVL >= 2 && Serial ) {
      Serial.print("gasNH3.hourlyAvg: "); Serial.println(gasNH3.hourlyAvg);
      Serial.print("gasCO.hourlyAvg: "); Serial.println(gasCO.hourlyAvg);
      Serial.print("gasNO2.hourlyAvg: "); Serial.println(gasNO2.hourlyAvg);
      Serial.print("gasC3H8.hourlyAvg: "); Serial.println(gasC3H8.hourlyAvg);
      Serial.print("gasC4H10.hourlyAvg: "); Serial.println(gasC4H10.hourlyAvg);
      Serial.print("gasCH4.hourlyAvg: "); Serial.println(gasCH4.hourlyAvg);
      Serial.print("gasH2.hourlyAvg: "); Serial.println(gasH2.hourlyAvg);
      Serial.print("gasC2H5OH.hourlyAvg: "); Serial.println(gasC2H5OH.hourlyAvg);
    }
  #endif
}
void encode_sendHourlyData() {
  union gasUnion  //Used to convert float to bytes[4] adapted from: http://www.cplusplus.com/forum/beginner/18566/
  {
    float gasFloat;
    unsigned char gasBytes[4];
  };
  gasUnion a;
  byte hourlyPayLoad[12]{0};

  a.gasFloat = gasNO2.hourlyMin;
  hourlyPayLoad[0] = a.gasBytes[0];
  hourlyPayLoad[1] = a.gasBytes[1];
  hourlyPayLoad[2] = a.gasBytes[2];
  hourlyPayLoad[3] = a.gasBytes[3];

  a.gasFloat = gasNO2.hourlyMax;
  hourlyPayLoad[4] = a.gasBytes[0];
  hourlyPayLoad[5] = a.gasBytes[1];
  hourlyPayLoad[6] = a.gasBytes[2];
  hourlyPayLoad[7] = a.gasBytes[3];

  a.gasFloat = gasNO2.hourlyAvg;
  hourlyPayLoad[8] = a.gasBytes[0];
  hourlyPayLoad[9] = a.gasBytes[1];
  hourlyPayLoad[10] = a.gasBytes[2];
  hourlyPayLoad[11] = a.gasBytes[3];

  broadcastData(webGatewayAddress, hourlyPayLoad, sizeof(hourlyPayLoad));
  resetFiveMinAvgs();
  resetHourlyMinMaxAvg();
}
void resetFiveMinAvgs() {
  for ( int i = 0; i < 11; i++ ) {  //reset
    gasNH3.fiveMinAvgs[i] = 0;
    gasCO.fiveMinAvgs[i] = 0;
    gasNO2.fiveMinAvgs[i] = 0;
    gasC3H8.fiveMinAvgs[i] = 0;
    gasC4H10.fiveMinAvgs[i] = 0;
    gasCH4.fiveMinAvgs[i] = 0;
    gasH2.fiveMinAvgs[i] = 0;
    gasC2H5OH.fiveMinAvgs[i] = 0;
  }
}
void resetHourlyMinMaxAvg(){
  gasNH3.hourlyMin = gasNH3.hourlyMax = gasNH3.hourlyAvg = 0;
  gasCO.hourlyMin = gasCO.hourlyMax = gasCO.hourlyAvg = 0;
  gasNO2.hourlyMin = gasNO2.hourlyMax = gasNO2.hourlyAvg = 0;
  gasC3H8.hourlyMin = gasC3H8.hourlyMax = gasC3H8.hourlyAvg = 0;
  gasC4H10.hourlyMin = gasC4H10.hourlyMax = gasC4H10.hourlyAvg = 0;
  gasCH4.hourlyMin = gasCH4.hourlyMax = gasCH4.hourlyAvg = 0;
  gasH2.hourlyMin = gasH2.hourlyMax = gasH2.hourlyAvg = 0;
  gasC2H5OH.hourlyMin = gasC2H5OH.hourlyMax = gasC2H5OH.hourlyAvg = 0;

}
void resetTwentySecondObsArray(){
  for (int i = 0; i < 15; i++) {
    gasNH3.twentySecondObs[i] = 0;
    gasCO.twentySecondObs[i] = 0;
    gasNO2.twentySecondObs[i] = 0;
    gasC3H8.twentySecondObs[i] = 0;
    gasC4H10.twentySecondObs[i] = 0;
    gasCH4.twentySecondObs[i] = 0;
    gasH2.twentySecondObs[i] = 0;
    gasC2H5OH.twentySecondObs[i] = 0;
    if ( i == 15 ) break;
  }
}
