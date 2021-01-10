void onLoRaReceive(int packetSize) {
  const byte localAddress {0x1};                // address of this device (0x1 is HEX "1")
  byte ReceivedAlarmStatus{0};
  byte ReceivedPayload{0};
  if (packetSize == 0) 
  {
    return; //bail if packet size == 0
  }
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length
  if (recipient != localAddress && recipient != 0xFF) {
    if ( outputLVL == 3 )
    {
      
    }
    return;                                       // skip rest of function
  }
  else if (incomingLength == byte(2) )  //standard format: two-byte payload
  {
    while (LoRa.available()) // can't use readString() in callback, so
    {            
      ReceivedAlarmStatus = LoRa.read();
      ReceivedPayload = LoRa.read();
    }
    if (ReceivedPayload == byte(247))          //status request
    {
      txStatusOnline();
    }
  }
}
