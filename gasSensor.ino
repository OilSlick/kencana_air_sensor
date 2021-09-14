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
#ifdef DEBUG
void printData() {
  Serial.println("+-------------------------+-------------------+--------+");
  Serial.println("|          Gas            |    Value (PPM)    |  High  |");
  Serial.println("+-------------------------+-------------------+--------+");

  Serial.print(F("| Amonia (NH3)            |  "));
  if( gasNH3.value >= 0 ) Serial.print(gasNH3.currentFiveMinAvg);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasNH3.value >= gasNH3.warn ) Serial.print("          X");
  Serial.println();
  //Amonia < 300ppm

  Serial.print(F("| Carbon monoxide (CO)    |  "));
  if( gasCO.value >= 0 ) Serial.print(gasCO.currentFiveMinAvg);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasCO.value >= gasCO.warn ) 
  {
    Serial.print("          X");
  }
  Serial.println();
  // want < 70ppm https://www.cacgas.com.au/blog/carbon-monoxide-co-toxic-gas-workplace-safety

  Serial.print(F("| Nitrous dioxide (NO2)   |  "));
  if( gasNO2.value >= 0 ) Serial.print( gasNO2.currentFiveMinAvg );
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasNO2.value >= gasNO2.warn ) Serial.print("          X");
  Serial.println();
  //want < 5000ppb https://www.cdc.gov/niosh/idlh/10102440.html

  Serial.print(F("| Propane (C3H8)          |  "));
  if( gasC3H8.value >= 0 ) Serial.print(gasC3H8.currentFiveMinAvg);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasC3H8.value >= gasC3H8.warn ) Serial.print("          X");
  Serial.println();
  //Propane < 2100PPM https://www.cdc.gov/niosh/idlh/74986.html (IDHL = Immediately Dangerous to Life or Health Concentrations)
  //More info on gases: https://safety.honeywell.com/content/dam/his-sandbox/products/gas-and-flame-detection/documents/Application-Note-202_The-ABC27s-Of-Gases-In-The-Industry_04-99.pdf

  Serial.print(F("| Butane (C4H10)          |  "));
  if( gasC4H10.value >= 0 ) Serial.print(gasC4H10.currentFiveMinAvg);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasC4H10.value >= gasC4H10.warn ) Serial.print("          X");
  Serial.println();
  //Butane < 1000ppm STEL (short term exposure limit of < 15 minutes) https://pubchem.ncbi.nlm.nih.gov/compound/Butane#section=Immediately-Dangerous-to-Life-or-Health-(IDLH)

  Serial.print(F("| Methane (CH4)           |  "));
  if( gasCH4.value >= 0 ) Serial.print(gasCH4.currentFiveMinAvg);
  else Serial.print("invalid");
  Serial.print(" ppb ");
  Serial.println();
  //methane no recommendations

  Serial.print(F("| Hydrogen gas (H2)       |  "));
  if( gasH2.value >= 0) Serial.print(gasH2.currentFiveMinAvg);
  else Serial.print("invalid");
  Serial.print(" ppb ");
  Serial.println();
  //hydrogen no recommendations

  Serial.print(F("| Ethyl alcohol (C2H5OH)  |  "));
  if( gasC2H5OH.value >= 0 ) Serial.print(gasC2H5OH.currentFiveMinAvg);
  else Serial.print("invalid");
  Serial.print(" ppb ");
  if( gasC2H5OH.value >= gasC2H5OH.warn ) Serial.print("          X");
  Serial.println();
  //ethyl alcohol < 3300 ppm
  Serial.println("+-------------------------+-------------------+--------+");
  Serial.println();
}
#endif
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
