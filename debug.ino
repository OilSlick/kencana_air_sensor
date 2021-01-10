#ifdef DEBUG
void displayDebug()
{
  if ( Serial )
  {
    Serial.println("+--------------------+------------------------+");  //https://ozh.github.io/ascii-tables/
    Serial.println("| This sensor        | kencana_air_sensor");
    Serial.printf("| LoRa address       | 0x%d\r\n", localAddress);
    Serial.printf("| outputLVL          | %d\r\n", outputLVL);
    Serial.printf("| TwentySecondCyclesCnt | %d\r\n", TwentySecondCyclesCnt);
    Serial.printf("| fiveMinCyclesCnt       | %d\r\n", fiveMinCyclesCnt); 
    Serial.println("+--------------------+------------------------+");
    Serial.println(" ");  
  }
  debugPrinted = true;
} //END displayDebug()

void handleSerial() {
 if (Serial.available() > 0) 
 {
  char incomingCharacter = Serial.read();
  if ( incomingCharacter == '1' ) txStatusOnline();
  if ( incomingCharacter == '2' ) cycleRed();
  if ( incomingCharacter == '3' ) cycleGreen();
  if ( incomingCharacter == '4' ) splitBlue();
  if ( incomingCharacter == '5' ) alarmRed();
  if ( incomingCharacter == 'd' ) displayDebug();
  if ( incomingCharacter == 'g' ) 
  {
    if ( gasI2Cerror == 0 ) getData();
    #ifdef DEBUG 
      if (Serial) printData();
    #endif
  }
  if ( incomingCharacter == 'h' )
  {
    Serial.println("d -> display debug");
    Serial.println("1 -> transmit status online");
    Serial.println("2 -> cycle red");
    Serial.println("3 -> cycle green");
    Serial.println("4 -> cycle blue");
    Serial.println("5 -> alarm red");
    Serial.println("g -> get gas data");
    Serial.println("h -> This Menu");
    Serial.println("r -> Reset");
    Serial.println("s -> Silence");
  }
  if ( incomingCharacter == 'r' ) setup();
  if ( incomingCharacter == 's' ) silence = true;
  if ( incomingCharacter == 't' ) 
  {
    getData();
    encodeData();
    broadcastData(webGatewayAddress, TransPayLoad);
    #ifdef DEBUG 
      printData();
    #endif
  }
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
#endif
