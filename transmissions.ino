void txStatusOnline()
{
  destination = broadcastAddress;
  MessagePayload[0] = byte(0);      //no alarm
  MessagePayload[1] = byte(255);    //status online
  broadcastMessage(destination, MessagePayload, sizeof(MessagePayload));
}
void txDayMode()
{
  destination = broadcastAddress;
  MessagePayload[0] = byte(0);        //no alarm
  MessagePayload[1] = byte(254);      //DayMode
  broadcastMessage(destination, MessagePayload, sizeof(MessagePayload));
}
void txNightMode()
{
  destination = broadcastAddress;
  MessagePayload[0] = byte(0);        //no alarm
  MessagePayload[1] = byte(251);      //NightMode
  broadcastMessage(destination, MessagePayload, sizeof(MessagePayload));
}
void txAwayMode()
{
  destination = broadcastAddress;
  MessagePayload[0] = byte(0);        //no alarm
  MessagePayload[1] = byte(250);      //AwayMode
  broadcastMessage(destination, MessagePayload, sizeof(MessagePayload));
}
void txSupressAlarm()
{
  destination = broadcastAddress;
  MessagePayload[0] = byte(0);        //no alarm
  MessagePayload[1] = byte(246);      //Suppress alarm
  broadcastMessage(destination, MessagePayload, sizeof(MessagePayload));
}
void txStatusRequest()
{
  destination = webGatewayAddress;
  MessagePayload[0] = byte(0);     //no-alarm
  MessagePayload[1] = byte(247);   //status-request
  broadcastMessage(destination, MessagePayload, sizeof(MessagePayload));
}
void broadcastData(byte destination, byte GasPayLoad[], int sizeofGasPayLoad) {
  delay(200);                           // may be needed?
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(sizeofGasPayLoad);        // add payload length
  LoRa.write(GasPayLoad, sizeofGasPayLoad);              // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
  LastSentTrans = byte(GasPayLoad[1]);
  if ( debug == true && Serial ) displayDebug("broadcastData()");
  digitalWrite(RFM95_SS, HIGH);
}
void broadcastMessage(byte destination, byte MessagePayload[], int sizeofMessagePayload) {
  delay(200);                           // may be needed?
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(sizeofMessagePayload);        // add payload length
  LoRa.write(MessagePayload, sizeofMessagePayload);              // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
  LastSentTrans = byte(MessagePayload[1]);
  if ( debug == true && Serial ) displayDebug("broadcastMessage()");
  digitalWrite(RFM95_SS, HIGH);
}
