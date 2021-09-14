#ifdef DEBUG
void displayDebug()
{
  if ( Serial )
  {
    Serial.println("+-----------------------+------------------------+");  //https://ozh.github.io/ascii-tables/
    Serial.println("| This sensor           | kencana_air_sensor");
     Serial.printf("| LoRa address          | 0x%d\r\n", localAddress);
     Serial.printf("| outputLVL             | %d\r\n", outputLVL);
     Serial.printf("| TwentySecondCyclesCnt | %d\r\n", TwentySecondCyclesCnt);
     Serial.printf("| fiveMinCyclesCnt      | %d\r\n", fiveMinCyclesCnt); 
    Serial.println("+-----------------------+------------------------+");
    Serial.println(" ");  
  }
  debugPrinted = true;
} //END displayDebug()

void handleSerial() {
 if (Serial.available() > 0) 
 {
  char incomingCharacter = Serial.read();
  if ( incomingCharacter == '1' ) {
    displayCurrentFiveMin();
    displayHourlyStats();
  }
  if ( incomingCharacter == '2' ) cycleRed();
  if ( incomingCharacter == '3' ) cycleGreen();
  if ( incomingCharacter == '4' ) splitBlue();
  if ( incomingCharacter == '5' ) alarmRed();
  if ( incomingCharacter == 'c' ) {
    Serial.print("gasI2Cerror: ");
    Wire.beginTransmission(gasI2Caddress);
    switch ( Wire.endTransmission() )
    {
      case 0:
        Serial.println("(0) success");
        break;
      case 1:
        Serial.println("(1) data too long to fit in transmit buffer");
        break;
      case 2:
        Serial.println("(2) received NACK on transmit of address");
        break;
      case 3:
        Serial.println("(3) received NACK on transmit of data");
        break;
      case 4:
        Serial.println("(4) other error");
        break;
    }
    
  }
  if ( incomingCharacter == 'd' ) displayDebug();
  if ( incomingCharacter == 'g' ) 
  {
    if ( gasI2Cerror == 0 ) getData(); 
      if (Serial) displayCurrentFiveMin();
  }
  if ( incomingCharacter == 'h' )
  {
    Serial.println("1 -> display data");
    Serial.println("2 -> cycle red");
    Serial.println("3 -> cycle green");
    Serial.println("4 -> cycle blue");
    Serial.println("5 -> alarm red");
    Serial.println("c -> check gas sensor");
    Serial.println("d -> display debug");
    Serial.println("g -> get gas data");
    Serial.println("h -> This Menu");
    Serial.println("r -> Reset");
    Serial.println("s -> Silence");
    Serial.println("t -> Transmit payload");
    Serial.println("o -> transmit status online");
  }
  if ( incomingCharacter == 'r' ) setup();
  if ( incomingCharacter == 's' ) silence = true;
  if ( incomingCharacter == 't' ) {
    getData();
    encodeData();
    broadcastData(webGatewayAddress, TransPayLoad, sizeof(TransPayLoad));
      displayCurrentFiveMin();
  }
  if ( incomingCharacter == 'o' ) txStatusOnline();
 }
}
void logReceipt(byte sender, byte recipient, byte incomingMsgId, byte incomingLength) {
  if ( Serial )
  {
    Serial.println("======Message Metadata======");
    Serial.println("Sender: 0x" + String(sender));
    Serial.println("Recipient: 0x" + String(recipient));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("Message length: " + String(incomingLength));
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println("============================");
   }
} //END logReceipt()
void displayHourlyStats(){
      Serial.println("+-------------------------+-----------+-----------+-----------+");
      Serial.println("|          Gas            |    min    |    Max    |    AVG    |");
      Serial.println("+-------------------------+-----------+-----------+-----------+");

      Serial.print(F("| Amonia (NH3)            |  "));
      Serial.print(gasNH3.hourlyMin); 
      Serial.print(gasNH3.hourlyMax); 
      Serial.println(gasNH3.hourlyAvg); 

      Serial.print(F("| Carbon monoxide (CO)    |  "));
      Serial.print(gasCO.hourlyMin);
      Serial.print(gasCO.hourlyMax);
      Serial.println(gasCO.hourlyAvg);

      Serial.print(F("| Nitrous dioxide (NO2)   |  "));
      Serial.print( gasNO2.hourlyMin );
      Serial.print( gasNO2.hourlyMax );
      Serial.println( gasNO2.hourlyAvg );

      Serial.print(F("| Propane (C3H8)          |  "));
      Serial.print(gasC3H8.hourlyMin);
      Serial.print(gasC3H8.hourlyMax);
      Serial.println(gasC3H8.hourlyAvg);
      
      Serial.print(F("| Butane (C4H10)          |  "));
      Serial.print(gasC4H10.hourlyMin);
      Serial.print(gasC4H10.hourlyMax);
      Serial.println(gasC4H10.hourlyAvg);

      Serial.print(F("| Methane (CH4)           |  "));
      Serial.print(gasCH4.hourlyMin);
      Serial.print(gasCH4.hourlyMax);
      Serial.println(gasCH4.hourlyAvg);

      Serial.print(F("| Hydrogen gas (H2)       |  "));
      Serial.print(gasH2.hourlyMin);
      Serial.print(gasH2.hourlyMax);
      Serial.println(gasH2.hourlyAvg);

      Serial.print(F("| Ethyl alcohol (C2H5OH)  |  "));
      Serial.print(gasC2H5OH.hourlyMin);
      Serial.print(gasC2H5OH.hourlyMax);
      Serial.println(gasC2H5OH.hourlyAvg);
      Serial.println("+-------------------------+-----------+-----------+-----------+");
      Serial.println();
}
void displayCurrentFiveMin() {
  Serial.println("+-------------------------+-------------------+--------+");
  Serial.println("|          Gas            |    Value (ppb)    |  High  |");
  Serial.println("+-------------------------+-------------------+--------+");

  Serial.print(F("| Amonia (NH3)            |  "));
  if( gasNH3.value >= 0 ) Serial.print(gasNH3.currentFiveMinAvg);
  else Serial.print("invalid");
  if( gasNH3.value >= gasNH3.warn ) Serial.print("          X");
  Serial.println();
  //Amonia < 300ppm

  Serial.print(F("| Carbon monoxide (CO)    |  "));
  if( gasCO.value >= 0 ) Serial.print(gasCO.currentFiveMinAvg);
  else Serial.print("invalid");
  if( gasCO.value >= gasCO.warn ) 
  {
    Serial.print("          X");
  }
  Serial.println();
  // want < 70ppm https://www.cacgas.com.au/blog/carbon-monoxide-co-toxic-gas-workplace-safety

  Serial.print(F("| Nitrous dioxide (NO2)   |  "));
  if( gasNO2.value >= 0 ) Serial.print( gasNO2.currentFiveMinAvg );
  else Serial.print("invalid");
  if( gasNO2.value >= gasNO2.warn ) Serial.print("          X");
  Serial.println();
  //want < 5000ppb https://www.cdc.gov/niosh/idlh/10102440.html

  Serial.print(F("| Propane (C3H8)          |  "));
  if( gasC3H8.value >= 0 ) Serial.print(gasC3H8.currentFiveMinAvg);
  else Serial.print("invalid");
  if( gasC3H8.value >= gasC3H8.warn ) Serial.print("          X");
  Serial.println();
  //Propane < 2100PPM https://www.cdc.gov/niosh/idlh/74986.html (IDHL = Immediately Dangerous to Life or Health Concentrations)
  //More info on gases: https://safety.honeywell.com/content/dam/his-sandbox/products/gas-and-flame-detection/documents/Application-Note-202_The-ABC27s-Of-Gases-In-The-Industry_04-99.pdf

  Serial.print(F("| Butane (C4H10)          |  "));
  if( gasC4H10.value >= 0 ) Serial.print(gasC4H10.currentFiveMinAvg);
  else Serial.print("invalid");
  if( gasC4H10.value >= gasC4H10.warn ) Serial.print("          X");
  Serial.println();
  //Butane < 1000ppm STEL (short term exposure limit of < 15 minutes) https://pubchem.ncbi.nlm.nih.gov/compound/Butane#section=Immediately-Dangerous-to-Life-or-Health-(IDLH)

  Serial.print(F("| Methane (CH4)           |  "));
  if( gasCH4.value >= 0 ) Serial.print(gasCH4.currentFiveMinAvg);
  else Serial.print("invalid");
  Serial.println();
  //methane no recommendations

  Serial.print(F("| Hydrogen gas (H2)       |  "));
  if( gasH2.value >= 0) Serial.print(gasH2.currentFiveMinAvg);
  else Serial.print("invalid");
  Serial.println();
  //hydrogen no recommendations

  Serial.print(F("| Ethyl alcohol (C2H5OH)  |  "));
  if( gasC2H5OH.value >= 0 ) Serial.print(gasC2H5OH.currentFiveMinAvg);
  else Serial.print("invalid");
  if( gasC2H5OH.value >= gasC2H5OH.warn ) Serial.print("          X");
  Serial.println();
  //ethyl alcohol < 3300 ppm
  Serial.println("+-------------------------+-------------------+--------+");
  Serial.println();
}
#endif
