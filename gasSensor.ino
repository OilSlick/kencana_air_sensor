void getData() {
  gasUnion a;
  ValueNH3 = gas.measure_NH3();
  if ( ValueNH3 < 0 || ValueNH3 > 500 )
  {
    a.gasVal = 99999;
  }
  else 
  {
    a.gasVal = ValueNH3;
    GasPayLoad[0] = a.gasBytes[0];
    GasPayLoad[1] = a.gasBytes[1];
    GasPayLoad[2] = a.gasBytes[2];
    GasPayLoad[3] = a.gasBytes[3];
  }
     
  ValueCO = gas.measure_CO();
  if ( ValueCO < 0 || ValueCO > 1000 )
  {
    a.gasVal = 99999;
  }
  else
  {
    a.gasVal = ValueCO;
    GasPayLoad[4] = a.gasBytes[0];
    GasPayLoad[5] = a.gasBytes[1];
    GasPayLoad[6] = a.gasBytes[2];
    GasPayLoad[7] = a.gasBytes[3];
  }

  ValueNO2 = gas.measure_NO2();
  if ( ValueNO2 < 0.5 || ValueNO2 > 10 )
  {
    a.gasVal = 99999;
  }
  else 
  {
    a.gasVal = ValueNO2;
    GasPayLoad[8] = a.gasBytes[0];
    GasPayLoad[9] = a.gasBytes[1];
    GasPayLoad[10] = a.gasBytes[2];
    GasPayLoad[11] = a.gasBytes[3];
  }

  ValueC3H8 = gas.measure_C3H8();
  if ( ValueC3H8 < 0 )
  {
    a.gasVal = 99999;
  }
  else 
  {
    a.gasVal = ValueC3H8;
    GasPayLoad[12] = a.gasBytes[0];
    GasPayLoad[13] = a.gasBytes[1];
    GasPayLoad[14] = a.gasBytes[2];
    GasPayLoad[15] = a.gasBytes[3];
  }
  
  ValueC4H10 = gas.measure_C4H10();
  if ( ValueC4H10 < 0 )
  {
    a.gasVal = 99999;
  }
  else 
  {
    a.gasVal = ValueC4H10;
    GasPayLoad[16] = a.gasBytes[0];
    GasPayLoad[17] = a.gasBytes[1];
    GasPayLoad[18] = a.gasBytes[2];
    GasPayLoad[19] = a.gasBytes[3];
  }

  ValueCH4 = gas.measure_CH4();
  if ( ValueCH4 < 0 )
  {
    a.gasVal = 99999;
  }
  else 
  {
    a.gasVal = ValueCH4;
    GasPayLoad[20] = a.gasBytes[0];
    GasPayLoad[21] = a.gasBytes[1];
    GasPayLoad[22] = a.gasBytes[2];
    GasPayLoad[23] = a.gasBytes[3];
  }
  
  ValueH2 = gas.measure_H2();
  if ( ValueH2 < 1 || ValueH2 > 1000 )
  {
    a.gasVal = 99999;
  }
  else 
  {
    a.gasVal = ValueH2;
    GasPayLoad[24] = a.gasBytes[0];
    GasPayLoad[25] = a.gasBytes[1];
    GasPayLoad[26] = a.gasBytes[2];
    GasPayLoad[27] = a.gasBytes[3];
  }
  
  ValueC2H5OH = gas.measure_C2H5OH();
  if ( ValueC2H5OH < 10 || ValueC2H5OH > 500 )
  {
    a.gasVal = 99999;
  }
  else 
  {
    a.gasVal = ValueC2H5OH;
    GasPayLoad[28] = a.gasBytes[0];
    GasPayLoad[29] = a.gasBytes[1];
    GasPayLoad[30] = a.gasBytes[2];
    GasPayLoad[31] = a.gasBytes[3];
  }
  
  if (transmitRequested == 1 )
  {
    destination = webGatewayAddress;
    broadcastData(destination, GasPayLoad, sizeof(GasPayLoad));
    transmitRequested = 0;
  }

}
void printData() {
  Serial.print(F("Amonia (NH3): "));
  if( ValueNH3 >= 0 ) Serial.print(ValueNH3);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( ValueNH3 >= 200 ) Serial.print(" (value high)");
  Serial.println();
  //Amonia < 300ppm

  Serial.print(F("carbon monoxide (CO): "));
  if( ValueCO >= 0 ) Serial.print(ValueCO);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( ValueCO >= COwarn ) 
  {
    Serial.print(" (value high)");
  }
  Serial.println();
  // want < 70ppm https://www.cacgas.com.au/blog/carbon-monoxide-co-toxic-gas-workplace-safety

  Serial.print(F("Nitrous dioxide (NO2): "));
  if( ValueNO2 >= 0 ) Serial.print( ValueNO2 );
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( ValueNO2 >= 4 ) Serial.print(" (value high)");
  Serial.println();
  //want < 5ppm

  Serial.print(F("Propane (C3H8): "));
  if( ValueC3H8 >= 0 ) Serial.print(ValueC3H8);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( ValueC3H8 >= propaneWarn ) Serial.print(" (value high)");
  Serial.println();
  //Propane < 2100PPM https://www.cdc.gov/niosh/idlh/74986.html (IDHL = Immediately Dangerous to Life or Health Concentrations)
  //More info on gases: https://safety.honeywell.com/content/dam/his-sandbox/products/gas-and-flame-detection/documents/Application-Note-202_The-ABC27s-Of-Gases-In-The-Industry_04-99.pdf

  Serial.print(F("Butane (C4H10): "));
  if( ValueC4H10 >= 0 ) Serial.print(ValueC4H10);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( ValueC4H10 >= 900 ) Serial.print(" (value high)");
  Serial.println();
  //Butane < 1000ppm STEL (short term exposure limit of < 15 minutes) https://pubchem.ncbi.nlm.nih.gov/compound/Butane#section=Immediately-Dangerous-to-Life-or-Health-(IDLH)

  Serial.print(F("Methane (CH4): "));
  if( ValueCH4 >= 0 ) Serial.print(ValueCH4);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println();
  //methane no recommendations

  Serial.print(F("Hydrogen gas (H2): "));
  if( ValueH2 >= 0) Serial.print(ValueH2);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println();
  //hydrogen no recommendations

  Serial.print(F("Ethyl alcohol (C2H5OH): "));
  if( ValueC2H5OH >= 0 ) Serial.print(ValueC2H5OH);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( ValueC2H5OH >= 2500 ) Serial.print(" (value high)");
  Serial.println();
  //ethyl alcohol < 3300 ppm
  Serial.println();
  Serial.println();
  Serial.println();
}
