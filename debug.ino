void displayDebug(String debugMessage)
{
  if ( Serial )
  {
    Serial.println("This sensor: kencana air sensor");
    Serial.println("LoRa address: 0x" + String(localAddress, HEX));
    Serial.println("debug status: " + String(debug));
    Serial.println("LastSentTrans: " + String(LastSentTrans));
    Serial.print("Gas Sensor Firmware Version: ");
    Serial.println(gasFirmwareversion);
    Serial.println("nightMode: " + String(nightMode));
    Serial.println("awayMode: " + String(awayMode));  
    Serial.println(" ");
    Serial.println("Debug Message: " + String(debugMessage));
    Serial.println(" "); 
    Serial.println("-----");
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
  if ( incomingCharacter == '4' ) cycleBlue();
  if ( incomingCharacter == '5' ) alarmRed();
  if ( incomingCharacter == '6' ) txNightMode();
  if ( incomingCharacter == '7' ) txDayMode();
  if ( incomingCharacter == '8' ) txSupressAlarm();
  if ( incomingCharacter == 'd' ) displayDebug("requested via console");
  if ( incomingCharacter == 'g' ) 
  {
    if ( gasI2Cerror == 0 ) getData();
    printData();
  }
  if ( incomingCharacter == 'h' )
  {
    Serial.println("d -> display debug");
    Serial.println("1 -> transmit status online");
    Serial.println("2 -> cycle red");
    Serial.println("3 -> cycle green");
    Serial.println("4 -> cycle blue");
    Serial.println("5 -> alarm red");
    Serial.println("6 -> transmit nightMode");
    Serial.println("7 -> transmit dayMode");
    Serial.println("8 -> transmit suppress alarm");
    Serial.println("g -> get gas data");
    Serial.println("h -> This Menu");
    Serial.println("r -> Reset");
  }
  if ( incomingCharacter == 'r' ) setup();
  if ( incomingCharacter == 't' ) 
  {
    transmitRequested = 1;
    if ( gasI2Cerror == 0 ) getData();
    printData();
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
