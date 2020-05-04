void onLoRaReceive(int packetSize) {
  if (packetSize == 0) 
  {
    if ( messageReceived == true )
    {
      messageReceived = false;
      if ( debug == true && Serial )
      {
        displayDebug("LoRaReceive packetSize = 0");
      }
    }
    return; //bail if packet size == 0
  }
  messageReceived = 1;

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length
  if ( debug != 0 )
    {
      logReceipt(sender, recipient, incomingMsgId, incomingLength);
    }
  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    if ( debug == true )
    {
      if ( Serial ) {
        Serial.println("This message is not for me.");
      }
    }
    return;                             // skip rest of function
  }

  if (incomingLength == byte(2) )  //standard format: two-byte payload
  {
    while (LoRa.available()) // can't use readString() in callback, so
    {            
      ReceivedAlarmStatus = bool(LoRa.read());
      ReceivedPayload = LoRa.read();
      LastReceivedTrans = ReceivedPayload;
    }
    if (ReceivedPayload == byte(254))         //DayMode
    {
      suppressAlarm = true;
      alarmReceived = false;
      //awayAlarmReceived = false;
      nightMode = false;
      awayMode = false;
      messageReceived = true;
      expectingMessage = false;
    }
    else if (ReceivedPayload == byte(251))    //NightMode
    {
      expectingMessage = false;
      nightMode = true;
      awayMode = false;
      suppressAlarm = false;
    }
    else if (ReceivedPayload == byte(250))    //AwayMode
    {
      awayMode = true;
      nightMode = false;
      messageReceived = true;
      expectingMessage = false;
    }
    else if (ReceivedPayload == byte(247))    //status request
    {
      delay(2500);
      destination = byte(sender);             //reply addressed to sender
      txStatusOnline();
      destination = 0xFF;                     //reset to broadcast
    }
    else if (ReceivedPayload == byte(246))    //suppress alarm
    {
      suppressAlarm = true;
      alarmReceived = false;
      //awayAlarmReceived = false;
      messageReceived = true;
      expectingMessage = false;
    }
    else if (ReceivedPayload == byte(245))    //enable debug
    {
      debug = 1;
    }
    else if (ReceivedPayload == byte(244))    //disable debug
    {
      debug = 0;
    }
    else 
      {
        messageReceived = false;
      }
    if ( debug != 0 )
    {
      logReceipt(sender, recipient, incomingMsgId, incomingLength);
    } 
    return;
  }
} //END onLoRaReceive(int packetSize)
