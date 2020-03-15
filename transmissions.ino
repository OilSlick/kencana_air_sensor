void txStatusActive()
{
  destination = webGatewayAddress;
  String outgoingMsg = "Air Sensor Online";  //PIR online
  broadcastMessage(outgoingMsg);
}

void broadcastMessage(String outgoingMsg)
{
  destination = broadcastAddress;
  delay(200);                           // may be needed?
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoingMsg.length());     // add payload length
  LoRa.print(outgoingMsg);              // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
  LastSentTrans = outgoingMsg;
  if ( debug == true && Serial ) displayDebug("broadcastMessage()");
  digitalWrite(RFM95_SS, HIGH);
  //radio.sleep(); //part of Airspayce's radiohead library
}
void broadcastData(byte fullpayload[]) {
  delay(200);                           // may be needed?
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(sizeof(fullpayload));        // add payload length
  LoRa.write(fullpayload, sizeof(fullpayload));              // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID

  if ( debug == true && Serial ) displayDebug("broadcastMessage()");
  digitalWrite(RFM95_SS, HIGH);
}
