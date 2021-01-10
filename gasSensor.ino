void getData() {
  if ( gasI2Cerror == 0 ) 
  {
    gasNH3.value = gas.measure_NH3();
    gasCO.value = gas.measure_CO();
    gasNO2.value = gas.measure_NO2();
    gasC3H8.value = gas.measure_C3H8();
    gasC4H10.value = gas.measure_C4H10();
    gasCH4.value = gas.measure_CH4();
    gasH2.value = gas.measure_H2();
    gasC2H5OH.value = gas.measure_C2H5OH(); 
  }
}
void buildFiveMinuteData(float currentValue, float (&twentySecondgas)[15], float (&currentFiveMinAvg), float (&hourlyMin), float (&hourlyMax) ) {
  twentySecondgas[TwentySecondCyclesCnt] = currentValue;
  if ( fiveMinCyclesCnt == 0 ) {     //hourly reset
    hourlyMin = currentValue;
    hourlyMax = currentValue;
    }
  if ( TwentySecondCyclesCnt != 0 ) 
  {
    float obsSum{0};
    for (int i = 0; i < TwentySecondCyclesCnt; i++) {
      obsSum = obsSum + twentySecondgas[i];
      if ( i == TwentySecondCyclesCnt ) break;
    }
    currentFiveMinAvg = obsSum / ( TwentySecondCyclesCnt + 1 );  //have to add 1 because the first obs is "0", second is "1", etc...
    if ( currentValue < hourlyMin ) hourlyMin = currentValue;
    if ( currentValue > hourlyMax ) hourlyMax = currentValue;
  }
}
void encodeData() {
  union gasUnion  //Used to convert float to bytes[4] adapted from: http://www.cplusplus.com/forum/beginner/18566/
  {
    float gasVal;
    unsigned char gasBytes[4];
  };
  gasUnion a;  
   
  a.gasVal = gasNH3.currentFiveMinAvg;
  GasPayLoad[0] = a.gasBytes[0];
  GasPayLoad[1] = a.gasBytes[1];
  GasPayLoad[2] = a.gasBytes[2];
  GasPayLoad[3] = a.gasBytes[3];
  
  a.gasVal = gasCO.currentFiveMinAvg;
  GasPayLoad[4] = a.gasBytes[0];
  GasPayLoad[5] = a.gasBytes[1];
  GasPayLoad[6] = a.gasBytes[2];
  GasPayLoad[7] = a.gasBytes[3];
   
  a.gasVal = gasNO2.currentFiveMinAvg;
  GasPayLoad[8] = a.gasBytes[0];
  GasPayLoad[9] = a.gasBytes[1];
  GasPayLoad[10] = a.gasBytes[2];
  GasPayLoad[11] = a.gasBytes[3];
  
  a.gasVal = gasC3H8.currentFiveMinAvg;
  GasPayLoad[12] = a.gasBytes[0];
  GasPayLoad[13] = a.gasBytes[1];
  GasPayLoad[14] = a.gasBytes[2];
  GasPayLoad[15] = a.gasBytes[3];
  
  a.gasVal = gasC4H10.currentFiveMinAvg;
  GasPayLoad[16] = a.gasBytes[0];
  GasPayLoad[17] = a.gasBytes[1];
  GasPayLoad[18] = a.gasBytes[2];
  GasPayLoad[19] = a.gasBytes[3];

  a.gasVal = gasCH4.currentFiveMinAvg;
  GasPayLoad[20] = a.gasBytes[0];
  GasPayLoad[21] = a.gasBytes[1];
  GasPayLoad[22] = a.gasBytes[2];
  GasPayLoad[23] = a.gasBytes[3];
  
  a.gasVal = gasH2.currentFiveMinAvg;
  GasPayLoad[24] = a.gasBytes[0];
  GasPayLoad[25] = a.gasBytes[1];
  GasPayLoad[26] = a.gasBytes[2];
  GasPayLoad[27] = a.gasBytes[3];
  
  a.gasVal = gasC2H5OH.currentFiveMinAvg;
  GasPayLoad[28] = a.gasBytes[0];
  GasPayLoad[29] = a.gasBytes[1];
  GasPayLoad[30] = a.gasBytes[2];
  GasPayLoad[31] = a.gasBytes[3];  
}

void transmitData() {
    destination = webGatewayAddress;
    broadcastData(destination, GasPayLoad, sizeof(GasPayLoad));
}
#ifdef DEBUG
void printData() {
  Serial.println("+-------------------------+-------------------+--------+---------+");
  Serial.println("|          Gas            |    Value (PPM)    |  High  | Invalid |");
  Serial.println("+-------------------------+-------------------+--------+---------+");

  Serial.print(F("| Amonia (NH3)            |  "));
  if( gasNH3.value >= 0 ) Serial.print(gasNH3.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasNH3.value >= 200 ) Serial.print(" (value high)");
  Serial.println();
  //Amonia < 300ppm

  Serial.print(F("| Carbon monoxide (CO)    |  "));
  if( gasCO.value >= 0 ) Serial.print(gasCO.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasCO.value >= gasCO.warn ) 
  {
    Serial.print(" (value high)");
  }
  Serial.println();
  // want < 70ppm https://www.cacgas.com.au/blog/carbon-monoxide-co-toxic-gas-workplace-safety

  Serial.print(F("| Nitrous dioxide (NO2)   |  "));
  if( gasNO2.value >= 0 ) Serial.print( gasNO2.value );
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasNO2.value >= 4 ) Serial.print("          X");
  Serial.println();
  //want < 5ppm https://www.cdc.gov/niosh/idlh/10102440.html

  Serial.print(F("| Propane (C3H8)          |  "));
  if( gasC3H8.value >= 0 ) Serial.print(gasC3H8.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasC3H8.value >= gasC3H8.warn ) Serial.print("          X");
  Serial.println();
  //Propane < 2100PPM https://www.cdc.gov/niosh/idlh/74986.html (IDHL = Immediately Dangerous to Life or Health Concentrations)
  //More info on gases: https://safety.honeywell.com/content/dam/his-sandbox/products/gas-and-flame-detection/documents/Application-Note-202_The-ABC27s-Of-Gases-In-The-Industry_04-99.pdf

  Serial.print(F("| Butane (C4H10)          |  "));
  if( gasC4H10.value >= 0 ) Serial.print(gasC4H10.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasC4H10.value >= 900 ) Serial.print("          X");
  Serial.println();
  //Butane < 1000ppm STEL (short term exposure limit of < 15 minutes) https://pubchem.ncbi.nlm.nih.gov/compound/Butane#section=Immediately-Dangerous-to-Life-or-Health-(IDLH)

  Serial.print(F("| Methane (CH4)           |  "));
  if( gasCH4.value >= 0 ) Serial.print(gasCH4.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println();
  //methane no recommendations

  Serial.print(F("| Hydrogen gas (H2)       |  "));
  if( gasH2.value >= 0) Serial.print(gasH2.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println();
  //hydrogen no recommendations

  Serial.print(F("| Ethyl alcohol (C2H5OH)  |  "));
  if( gasC2H5OH.value >= 0 ) Serial.print(gasC2H5OH.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasC2H5OH.value >= 2500 ) Serial.print(" (value high)");
  Serial.println();
  //ethyl alcohol < 3300 ppm
  Serial.println("+-------------------------+-------------------+--------+---------+");
  Serial.println();
  Serial.println();
}
#endif

bool isValid(int gas_id, float gas_value) {
  if ( gas_id == 1 ) 
  {
    if ( gasNH3.value < 0 || ( gasNH3.value > 0 && gasNH3.value < gasNH3.minDetectable ) || gasNH3.value > gasNH3.maxDetectable )
    {
      return false;
    }
    else return true;
  }
  if ( gas_id == 2 ) 
  {
    if ( gasCO.value < 0 || gasCO.value > 1000 )
    {
      return false;
    }
    else return true;
  }
  
  if ( gas_id == 3 ) 
  {
    if ( gasNO2.value < 0.5 || gasNO2.value > 10 )
    {
      return false;
    }
    else return true;
  }

  if ( gas_id == 4 ) 
  {
    if ( gasC3H8.value < 0 )
    {
      return false;
    }
    else return true;
  }
  if ( gas_id == 5 ) 
  {
    if ( gasC4H10.value < 0 )
    {
      return false;
    }
    else return true;
  }
  if ( gas_id == 6 ) 
  {
    if ( gasCH4.value < 0 )
    {
      return false;
    }
    else return true;
  }
  if ( gas_id == 7 ) 
  {
    if ( gasH2.value < 1 || gasH2.value > 1000 )
    {
      return false;
    }
    else return true;
  }
  if ( gas_id == 8 ) 
  {
    if ( gasC2H5OH.value < 10 || gasC2H5OH.value > 500 )
    {
      return false;
    }
    else return true;
  }
}
void processHourlyData() {
  for ( int i = 0; i < 12; i++ ) {
    gasNH3.hourlyAvg += gasNH3.fiveMinAvgs[i];
    gasCO.hourlyAvg += gasCO.fiveMinAvgs[i];
    gasNO2.hourlyAvg += gasNO2.fiveMinAvgs[i];
    gasC3H8.hourlyAvg += gasC3H8.fiveMinAvgs[i];
    gasC4H10.hourlyAvg += gasC4H10.fiveMinAvgs[i];
    gasCH4.hourlyAvg += gasCH4.fiveMinAvgs[i];
    gasH2.hourlyAvg += gasH2.fiveMinAvgs[i];
    gasC2H5OH.hourlyAvg += gasC2H5OH.fiveMinAvgs[i];
  }
  //#DEBUG:
  Serial.print("gasNH3.hourlyAvg: "); Serial.println(gasNH3.hourlyAvg);
  Serial.print("gasCO.hourlyAvg: "); Serial.println(gasCO.hourlyAvg);
  Serial.print("gasNO2.hourlyAvg: "); Serial.println(gasNO2.hourlyAvg);
  Serial.print("gasC3H8.hourlyAvg: "); Serial.println(gasC3H8.hourlyAvg);
  Serial.print("gasC4H10.hourlyAvg: "); Serial.println(gasC4H10.hourlyAvg);
  Serial.print("gasCH4.hourlyAvg: "); Serial.println(gasCH4.hourlyAvg);
  Serial.print("gasH2.hourlyAvg: "); Serial.println(gasH2.hourlyAvg);
  Serial.print("gasC2H5OH.hourlyAvg: "); Serial.println(gasC2H5OH.hourlyAvg);
}
