void txStatusOnline()
{
  destination = webGatewayAddress;
  fullpayload[0] = byte(0);      //no alarm
  fullpayload[1] = byte(255);    //status online
  broadcastData(destination, fullpayload, sizeof(fullpayload));
}
void txDayMode()
{
  destination = broadcastAddress;
  fullpayload[0] = byte(0);        //no alarm
  fullpayload[1] = byte(254);      //DayMode
  broadcastData(destination, fullpayload, sizeof(fullpayload));
}
void txNightMode()
{
  destination = broadcastAddress;
  fullpayload[0] = byte(0);        //no alarm
  fullpayload[1] = byte(251);      //NightMode
  broadcastData(destination, fullpayload, sizeof(fullpayload));
}
void txAwayMode()
{
  destination = broadcastAddress;
  fullpayload[0] = byte(0);        //no alarm
  fullpayload[1] = byte(250);      //AwayMode
  broadcastData(destination, fullpayload, sizeof(fullpayload));
}
void txSupressAlarm()
{
  destination = broadcastAddress;
  fullpayload[0] = byte(0);        //no alarm
  fullpayload[1] = byte(246);      //Suppress alarm
  broadcastData(destination, fullpayload, sizeof(fullpayload));
}
void txStatusRequest()
{
  destination = webGatewayAddress;
  fullpayload[0] = byte(0);     //no-alarm
  fullpayload[1] = byte(247);   //status-request
  broadcastData(destination, fullpayload, sizeof(fullpayload));
}
void broadcastData(byte destination, byte fullpayload[], int sizeofFullPayload) {
  delay(200);                           // may be needed?
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(sizeofFullPayload);        // add payload length
  LoRa.write(fullpayload, sizeofFullPayload);              // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
  LastSentTrans = byte(fullpayload[1]);
  if ( debug == true && Serial ) displayDebug("broadcastData()");
  digitalWrite(RFM95_SS, HIGH);
}
/* void broadcastData(byte fullpayload[], int sizeofFullPayload) {
  delay(200);                           // may be needed?
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(sizeofFullPayload);        // add payload length
  LoRa.write(fullpayload, sizeofFullPayload);              // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID

  if ( debug == true && Serial ) displayDebug("broadcastMessage()");
  digitalWrite(RFM95_SS, HIGH);
}*/
