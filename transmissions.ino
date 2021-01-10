void txStatusOnline()
{
  destination = webGatewayAddress;
  MessagePayload[0] = byte(0);      //no alarm
  MessagePayload[1] = byte(255);    //status online
  broadcastData(destination, MessagePayload);
}
void broadcastData(byte destination, byte TransPayLoad[]) {
  int sizeofGasPayLoad = sizeof(TransPayLoad);
  delay(200);                           // may be needed?
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(sizeofGasPayLoad);        // add payload length
  LoRa.write(TransPayLoad, sizeofGasPayLoad);              // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
  //LastSentTrans = byte(GasPayLoad[1]);
  #ifdef DEBUG
    if (Serial) displayDebug();
  #endif
  digitalWrite(RFM95_SS, HIGH);
}
