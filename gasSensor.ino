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
void encodeData() {
  gasUnion a;  
  if ( isValid(1, gasNH3.value) )
  {
    if ( gasNH3.value_is_valid == false ) gasNH3.value_is_valid = true;
    a.gasVal = gasNH3.value;
    GasPayLoad[0] = a.gasBytes[0];
    GasPayLoad[1] = a.gasBytes[1];
    GasPayLoad[2] = a.gasBytes[2];
    GasPayLoad[3] = a.gasBytes[3];
  }
  else
  {
    if ( gasNH3.value_is_valid == true ) gasNH3.value_is_valid = false;
    a.gasVal = 0;
    GasPayLoad[0] = a.gasBytes[0];
    GasPayLoad[1] = a.gasBytes[1];
    GasPayLoad[2] = a.gasBytes[2];
    GasPayLoad[3] = a.gasBytes[3];
  } 
  if ( isValid(2, gasCO.value) )
  {
    if ( gasCO.value_is_valid == false ) gasCO.value_is_valid = true;
    a.gasVal = gasCO.value;
    GasPayLoad[4] = a.gasBytes[0];
    GasPayLoad[5] = a.gasBytes[1];
    GasPayLoad[6] = a.gasBytes[2];
    GasPayLoad[7] = a.gasBytes[3];
  }
  else
  {
    if ( gasCO.value_is_valid == true ) gasCO.value_is_valid = false;
    a.gasVal = 0;
    GasPayLoad[4] = a.gasBytes[0];
    GasPayLoad[5] = a.gasBytes[1];
    GasPayLoad[6] = a.gasBytes[2];
    GasPayLoad[7] = a.gasBytes[3];
  }
  if ( isValid(3, gasNO2.value) )
  {
    if ( gasNO2.value_is_valid == false ) gasNO2.value_is_valid = true;
    a.gasVal = gasNO2.value;
    GasPayLoad[8] = a.gasBytes[0];
    GasPayLoad[9] = a.gasBytes[1];
    GasPayLoad[10] = a.gasBytes[2];
    GasPayLoad[11] = a.gasBytes[3];
  }
  else 
  {
    if ( gasNO2.value_is_valid == true ) gasNO2.value_is_valid = false;
    a.gasVal = 0;
    GasPayLoad[8] = a.gasBytes[0];
    GasPayLoad[9] = a.gasBytes[1];
    GasPayLoad[10] = a.gasBytes[2];
    GasPayLoad[11] = a.gasBytes[3];
  }

  if ( isValid(4, gasC3H8.value) )
  {
    if ( gasC3H8.value_is_valid == false ) gasC3H8.value_is_valid = true;
    a.gasVal = gasC3H8.value;
    GasPayLoad[12] = a.gasBytes[0];
    GasPayLoad[13] = a.gasBytes[1];
    GasPayLoad[14] = a.gasBytes[2];
    GasPayLoad[15] = a.gasBytes[3];
  }
  else 
  {
    if ( gasC3H8.value_is_valid == true ) gasC3H8.value_is_valid = false;
    a.gasVal = 0;
    GasPayLoad[12] = a.gasBytes[0];
    GasPayLoad[13] = a.gasBytes[1];
    GasPayLoad[14] = a.gasBytes[2];
    GasPayLoad[15] = a.gasBytes[3];
  }
  
  //gasC4H10.value = gas.measure_C4H10();
  //if ( gasC4H10.value < 0 )
  if ( isValid(5, gasC4H10.value) )
  {
    if ( gasC4H10.value_is_valid == false ) gasC4H10.value_is_valid = true;
    a.gasVal = gasC4H10.value;
    GasPayLoad[16] = a.gasBytes[0];
    GasPayLoad[17] = a.gasBytes[1];
    GasPayLoad[18] = a.gasBytes[2];
    GasPayLoad[19] = a.gasBytes[3];
  }
  else 
  {
    if ( gasC4H10.value_is_valid == true ) gasC4H10.value_is_valid = false;
    a.gasVal = 0;
    GasPayLoad[16] = a.gasBytes[0];
    GasPayLoad[17] = a.gasBytes[1];
    GasPayLoad[18] = a.gasBytes[2];
    GasPayLoad[19] = a.gasBytes[3];
  }

  //gasCH4.value = gas.measure_CH4();
  //if ( gasCH4.value < 0 )
  if ( isValid(6, gasCH4.value) )
  {
    if ( gasCH4.value_is_valid == true ) gasCH4.value_is_valid = false;
    a.gasVal = gasCH4.value;
    GasPayLoad[20] = a.gasBytes[0];
    GasPayLoad[21] = a.gasBytes[1];
    GasPayLoad[22] = a.gasBytes[2];
    GasPayLoad[23] = a.gasBytes[3];
  }
  else 
  {
    if ( gasCH4.value_is_valid == true ) gasCH4.value_is_valid = false;
    a.gasVal = 0;
    GasPayLoad[20] = a.gasBytes[0];
    GasPayLoad[21] = a.gasBytes[1];
    GasPayLoad[22] = a.gasBytes[2];
    GasPayLoad[23] = a.gasBytes[3];
  }
  
  //gasH2.value = gas.measure_H2();
  //if ( gasH2.value < 1 || gasH2.value > 1000 )
  if ( isValid(7, gasH2.value) )
  {
    a.gasVal = gasH2.value;
    GasPayLoad[24] = a.gasBytes[0];
    GasPayLoad[25] = a.gasBytes[1];
    GasPayLoad[26] = a.gasBytes[2];
    GasPayLoad[27] = a.gasBytes[3];
  }
  else 
  {
    if ( gasH2.value_is_valid == true ) gasH2.value_is_valid = false;
    a.gasVal = 0;
    GasPayLoad[24] = a.gasBytes[0];
    GasPayLoad[25] = a.gasBytes[1];
    GasPayLoad[26] = a.gasBytes[2];
    GasPayLoad[27] = a.gasBytes[3];
  }
  
  //gasC2H5OH.value = gas.measure_C2H5OH();
  //if ( gasC2H5OH.value < 10 || gasC2H5OH.value > 500 )
  if ( isValid(7, gasC2H5OH.value) )
  {
    if ( gasC2H5OH.value_is_valid == false ) gasC2H5OH.value_is_valid = true;
    a.gasVal = gasC2H5OH.value;
    GasPayLoad[28] = a.gasBytes[0];
    GasPayLoad[29] = a.gasBytes[1];
    GasPayLoad[30] = a.gasBytes[2];
    GasPayLoad[31] = a.gasBytes[3];
    Serial.print("Valid: ");
    Serial.println(gasCO.value_is_valid);
  }
  else 
  {
    if ( gasC2H5OH.value_is_valid == true ) gasC2H5OH.value_is_valid = false;
    a.gasVal = 0;
    GasPayLoad[28] = a.gasBytes[0];
    GasPayLoad[29] = a.gasBytes[1];
    GasPayLoad[30] = a.gasBytes[2];
    GasPayLoad[31] = a.gasBytes[3];
    Serial.print("Valid: ");
    Serial.println(gasCO.value_is_valid);
  }
}

void transmitData() {
    destination = webGatewayAddress;
    broadcastData(destination, GasPayLoad, sizeof(GasPayLoad));
    transmitRequested = 0;
}
  
void printData() {
  Serial.print(F("Amonia (NH3): "));
  if( gasNH3.value >= 0 ) Serial.print(gasNH3.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasNH3.value >= 200 ) Serial.print(" (value high)");
  Serial.println();
  //Amonia < 300ppm

  Serial.print(F("carbon monoxide (CO): "));
  if( gasCO.value >= 0 ) Serial.print(gasCO.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasCO.value >= gasCO.warn ) 
  {
    Serial.print(" (value high)");
  }
  Serial.println();
  // want < 70ppm https://www.cacgas.com.au/blog/carbon-monoxide-co-toxic-gas-workplace-safety

  Serial.print(F("Nitrous dioxide (NO2): "));
  if( gasNO2.value >= 0 ) Serial.print( gasNO2.value );
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasNO2.value >= 4 ) Serial.print(" (value high)");
  Serial.println();
  //want < 5ppm

  Serial.print(F("Propane (C3H8): "));
  if( gasC3H8.value >= 0 ) Serial.print(gasC3H8.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasC3H8.value >= gasC3H8.warn ) Serial.print(" (value high)");
  Serial.println();
  //Propane < 2100PPM https://www.cdc.gov/niosh/idlh/74986.html (IDHL = Immediately Dangerous to Life or Health Concentrations)
  //More info on gases: https://safety.honeywell.com/content/dam/his-sandbox/products/gas-and-flame-detection/documents/Application-Note-202_The-ABC27s-Of-Gases-In-The-Industry_04-99.pdf

  Serial.print(F("Butane (C4H10): "));
  if( gasC4H10.value >= 0 ) Serial.print(gasC4H10.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasC4H10.value >= 900 ) Serial.print(" (value high)");
  Serial.println();
  //Butane < 1000ppm STEL (short term exposure limit of < 15 minutes) https://pubchem.ncbi.nlm.nih.gov/compound/Butane#section=Immediately-Dangerous-to-Life-or-Health-(IDLH)

  Serial.print(F("Methane (CH4): "));
  if( gasCH4.value >= 0 ) Serial.print(gasCH4.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println();
  //methane no recommendations

  Serial.print(F("Hydrogen gas (H2): "));
  if( gasH2.value >= 0) Serial.print(gasH2.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println();
  //hydrogen no recommendations

  Serial.print(F("Ethyl alcohol (C2H5OH): "));
  if( gasC2H5OH.value >= 0 ) Serial.print(gasC2H5OH.value);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( gasC2H5OH.value >= 2500 ) Serial.print(" (value high)");
  Serial.println();
  //ethyl alcohol < 3300 ppm
  Serial.println();
  Serial.println();
  Serial.println();
}
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
