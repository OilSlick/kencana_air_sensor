void displayDebug(String debugMessage)
{
  if ( Serial )
  {
    Serial.println("This sensor: kencana air sensor");
    Serial.println("LoRa address: 0x" + String(localAddress, HEX));
    Serial.println("debug status: " + String(debug));
    Serial.println("LastSentTrans: " + String(LastSentTrans));
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
  if ( incomingCharacter == '2' ) txStatusOnline();
  if ( incomingCharacter == '5' ) txNightMode();
  if ( incomingCharacter == '8' ) txSupressAlarm();
  if ( incomingCharacter == 'd' ) displayDebug("requested via console");
  if ( incomingCharacter == 'g' ) 
  {
    getData();
    printData();
  }
  if ( incomingCharacter == 'h' )
  {
    Serial.println("d -> display debug");
    Serial.println("2 -> transmit status online");
    Serial.println("5 -> transmit nightMode");
    Serial.println("6 -> transmit auto NightMode");
    Serial.println("8 -> transmit suppress alarm");
    Serial.println("g -> get gas data");
    Serial.println("h -> This Menu");
  }
 }
}
