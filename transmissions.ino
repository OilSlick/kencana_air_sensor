void txStatusOnline()
{
  byte MessagePayload[2];
  destination = webGatewayAddress;
  MessagePayload[0] = byte(0);      //no alarm
  MessagePayload[1] = byte(255);    //status online
  broadcastData(destination, MessagePayload);
}
void broadcastData(byte destination, byte PayLoad[]) {
  int sizeofPayLoad = sizeof(PayLoad);
  delay(200);                           // may be needed?
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(sizeofPayLoad);            // add payload length
  LoRa.write(PayLoad, sizeofPayLoad);              // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
  #ifdef DEBUG
    if (Serial) displayDebug();
  #endif
  digitalWrite(RFM95_SS, HIGH);
}
