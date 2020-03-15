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
  String consoleMessage = String(incomingCharacter);
  if ( consoleMessage == "d" ) displayDebug("requested via console");
  if ( consoleMessage == "a" )
  {
    String outgoingMsg = "aaaaa";  //Test transmission
    broadcastMessage(outgoingMsg); 
  }
  if ( consoleMessage == "1" )
  {
    String outgoingMsg = "12121";  //"12121" = sound alarm while in awayMode
    broadcastMessage(outgoingMsg); 
  }
  if ( consoleMessage == "2" )
  {
    String outgoingMsg = "PIR Alarm";  
    broadcastMessage(outgoingMsg); 
  }
  if ( consoleMessage == "3" )
  {
    String outgoingMsg = "33333";  
    broadcastMessage(outgoingMsg); 
  }
  if ( consoleMessage == "4" )
  {
    String outgoingMsg = "44444";  
    broadcastMessage(outgoingMsg);
  }
  if ( consoleMessage == "5" )
  {
    String outgoingMsg = "55555";  
    broadcastMessage(outgoingMsg); 
  }
  if ( consoleMessage == "6" )
  {
    String outgoingMsg = "55555a";  
    broadcastMessage(outgoingMsg); 
  }
  if ( consoleMessage == "7" )
  {
    String outgoingMsg = "77777";  
    broadcastMessage(outgoingMsg); 
  }
  if ( consoleMessage == "8" )
  {
    String outgoingMsg = "88888";  
    broadcastMessage(outgoingMsg); 
  }
  if ( consoleMessage == "9" )
  {
    String outgoingMsg = "99999";  
    broadcastMessage(outgoingMsg); 
  }
  if ( consoleMessage == "g" )
  {
    getData();  
    printData(); 
  }
  if ( consoleMessage == "h" )
  {
    printMenu();
  }
  if ( consoleMessage == "t" )
  {
    transmitRequested = 1;
    getData();
  }
 }
} //END handleSerial()

void printMenu() {
    Serial.println("1 -> transmit '12121'");
    Serial.println("2 -> transmit 'PIR Alarm'");
    Serial.println("3 -> transmit '33333'");
    Serial.println("4 -> transmit '44444'");
    Serial.println("5 -> transmit '55555'");
    Serial.println("6 -> transmit '55555a'");
    Serial.println("7 -> transmit '77777'");
    Serial.println("8 -> transmit '88888'");
    Serial.println("9 -> transmit '99999'");
    Serial.println("a -> transmit 'aaaaa'");
    Serial.println("d -> display debug");
    Serial.println("g -> Get sensor data and print it to serial");
    Serial.println("h -> This Menu");
    Serial.println("t -> Transmit sensor data");
}
