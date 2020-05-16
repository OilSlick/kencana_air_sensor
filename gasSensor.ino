void getData() {
  ValueNH3 = gas.measure_NH3();
  convertedValue = ValueNH3 * 100;
  GasPayLoad[0] = highByte(convertedValue);
  GasPayLoad[1] = lowByte(convertedValue);
    
  ValueCO = gas.measure_CO();
  convertedValue = ValueCO * 100;
  GasPayLoad[2] = highByte(convertedValue);
  GasPayLoad[3] = lowByte(convertedValue);

  ValueNO2 = gas.measure_NO2();
  convertedValue = ValueNO2 * 100;
  GasPayLoad[4] = highByte(convertedValue);
  GasPayLoad[5] = lowByte(convertedValue);

  ValueC3H8 = gas.measure_C3H8();
  convertedValue = ValueC3H8 * 100;
  GasPayLoad[6] = highByte(convertedValue);
  GasPayLoad[7] = lowByte(convertedValue);
  
  ValueC4H10 = gas.measure_C4H10();
  convertedValue = ValueC4H10 * 100;
  GasPayLoad[8] = highByte(convertedValue);
  GasPayLoad[9] = lowByte(convertedValue);

  ValueCH4 = gas.measure_CH4();
  convertedValue = ValueCH4 * 100;
  GasPayLoad[10] = highByte(convertedValue);
  GasPayLoad[11] = lowByte(convertedValue);

  ValueH2 = gas.measure_H2();
  convertedValue = ValueH2 * 100;
  GasPayLoad[12] = highByte(convertedValue);
  GasPayLoad[13] = lowByte(convertedValue);
  
  ValueC2H5OH = gas.measure_C2H5OH();
  convertedValue = ValueC2H5OH * 100;
  GasPayLoad[14] = highByte(convertedValue);
  GasPayLoad[15] = lowByte(convertedValue);

  if (transmitRequested == 1 )
  {
    destination = webGatewayAddress;
    broadcastData(destination, GasPayLoad, sizeof(GasPayLoad));
    transmitRequested = 0;
  }

}
void printData() {
  Serial.print(F("Amonia (NH3): "));
  //decodedValue = (GasPayLoad[0] << 8) + GasPayLoad[1];
  //ValueNH3 = decodedValue /100;
  if( ValueNH3 >= 0 ) Serial.print(ValueNH3);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( ValueNH3 >= 200 ) Serial.print(" (value high)");
  Serial.println();
  //Amonia < 300ppm

  Serial.print(F("carbon monoxide (CO): "));
  //decodedValue = (GasPayLoad[2] << 8) + GasPayLoad[3];
  //ValueCO = decodedValue /100;
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
  //decodedValue = (GasPayLoad[4] << 8) + GasPayLoad[5];
  //ValueNO2 = decodedValue /100;
  if( ValueNO2 >= 0 ) Serial.print( ValueNO2 );
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( ValueNO2 >= 4 ) Serial.print(" (value high)");
  Serial.println();
  //want < 5ppm

  Serial.print(F("Propane (C3H8): "));
  //decodedValue = (GasPayLoad[6] << 8) + GasPayLoad[7];
  //ValueC3H8 = decodedValue /100;
  if( ValueC3H8 >= 0 ) Serial.print(ValueC3H8);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( ValueC3H8 >= propaneWarn ) Serial.print(" (value high)");
  Serial.println();
  //Propane < 2100PPM https://www.cdc.gov/niosh/idlh/74986.html (IDHL = Immediately Dangerous to Life or Health Concentrations)
  //More info on gases: https://safety.honeywell.com/content/dam/his-sandbox/products/gas-and-flame-detection/documents/Application-Note-202_The-ABC27s-Of-Gases-In-The-Industry_04-99.pdf

  Serial.print(F("Butane (C4H10): "));
  //decodedValue = (GasPayLoad[8] << 8) + GasPayLoad[9];
  //ValueC4H10 = decodedValue /100;
  if( ValueC4H10 >= 0 ) Serial.print(ValueC4H10);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  if( ValueC4H10 >= 900 ) Serial.print(" (value high)");
  Serial.println();
  //Butane < 1000ppm STEL (short term exposure limit of < 15 minutes) https://pubchem.ncbi.nlm.nih.gov/compound/Butane#section=Immediately-Dangerous-to-Life-or-Health-(IDLH)

  Serial.print(F("Methane (CH4): "));
  //decodedValue = (GasPayLoad[10] << 8) + GasPayLoad[11];
  //ValueCH4 = decodedValue /100;
  if( ValueCH4 >= 0 ) Serial.print(ValueCH4);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println();
  //methane no recommendations

  Serial.print(F("Hydrogen gas (H2): "));
  //decodedValue = (GasPayLoad[12] << 8) + GasPayLoad[13];
  //ValueH2 = decodedValue /100;
  if( ValueH2 >= 0) Serial.print(ValueH2);
  else Serial.print("invalid");
  Serial.print(" ppm ");
  Serial.println();
  //hydrogen no recommendations

  Serial.print(F("Ethyl alcohol (C2H5OH): "));
  //decodedValue = (GasPayLoad[14] << 8) + GasPayLoad[15];
  //ValueC2H5OH = decodedValue /100;
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
