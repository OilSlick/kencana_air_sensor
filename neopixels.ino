void cycleRed()
{
  //cycle up
  for(int i=0; i<numPixels; i++) { // For each pixel...
    // pixels->Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels->setPixelColor(i, pixels->Color(50, 0, 0));
    pixels->show();   // Send the updated pixel colors to the hardware.
    delay(30);
    if ( i == numPixels ) break;
    pixels->setPixelColor(i, pixels->Color(0, 0, 0));
    pixels->show();
    delay(30);
  }
  //cycle down
  for(int i=numPixels; i<=numPixels; i--) { // For each pixel...
    // pixels->Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    if ( i < numPixels )
    {
      pixels->setPixelColor(i, pixels->Color(50, 0, 0));
      pixels->show();   // Send the updated pixel colors to the hardware.
      delay(30);
    }
    pixels->setPixelColor(i, pixels->Color(0, 0, 0));
    pixels->show();
    delay(30);
    if ( i == 0 ) break;
  }
  pixels->clear();
}
void cycleGreen()
{
  for(int i=0; i<numPixels; i++) { // For each pixel...
    // pixels->Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels->setPixelColor(i, pixels->Color(0, 50, 0));
    pixels->show();   // Send the updated pixel colors to the hardware.
    delay(30);
    if ( i == numPixels ) break;
    pixels->setPixelColor(i, pixels->Color(0, 0, 0));
    pixels->show();
    delay(30);
  }
  for(int i=numPixels; i<=numPixels; i--) { // For each pixel...
    // pixels->Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    if ( i < numPixels )
    {
      pixels->setPixelColor(i, pixels->Color(0, 50, 0));
      pixels->show();   // Send the updated pixel colors to the hardware.
      delay(30);
    }
    pixels->setPixelColor(i, pixels->Color(0, 0, 0));
    pixels->show();
    delay(30);
    if ( i == 0 ) break;
  }
  pixels->clear();
}
void splitBlue()
{
  //for loop with two variables found here: https://thispointer.com/for-loop-with-2-variables-in-c-and-java/
  for(int a = 4, b = 3; a < numPixels; a++, b--) { // For each pixel...
    // pixels->Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels->setPixelColor(a, pixels->Color(0, 0, 50));
    pixels->setPixelColor(b, pixels->Color(0, 0, 50));
    pixels->show();   // Send the updated pixel colors to the hardware.
    delay(30);
    if ( a == numPixels ) break;
    pixels->setPixelColor(a, pixels->Color(0, 0, 0));
    pixels->setPixelColor(b, pixels->Color(0, 0, 0));
    pixels->show();
    delay(30);
  } 
  for(int a = numPixels, b = 0; a >= 4; a--, b++) { // For each pixel...
    // pixels->Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    if ( a < numPixels )
    {
      pixels->setPixelColor(a, pixels->Color(0, 0, 50));
      pixels->setPixelColor(b, pixels->Color(0, 0, 50));
      pixels->show();   // Send the updated pixel colors to the hardware.
      delay(30);
    }
    pixels->setPixelColor(a, pixels->Color(0, 0, 0));
    pixels->setPixelColor(b, pixels->Color(0, 0, 0));
    pixels->show();
    delay(30);
    if ( a == 0 ) break;
  }
  pixels->clear();
}
void splitYellow()
{
  //for loop with two variables found here: https://thispointer.com/for-loop-with-2-variables-in-c-and-java/
  for(int a = 4, b = 3; a < numPixels; a++, b--) { // For each pixel...
    // pixels->Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels->setPixelColor(a, pixels->Color(50, 50, 0));
    pixels->setPixelColor(b, pixels->Color(50, 50, 0));
    pixels->show();   // Send the updated pixel colors to the hardware.
    delay(30);
    if ( a == numPixels ) break;
    pixels->setPixelColor(a, pixels->Color(0, 0, 0));
    pixels->setPixelColor(b, pixels->Color(0, 0, 0));
    pixels->show();
    delay(30);
  } 
  for(int a = numPixels, b = 0; a >= 4; a--, b++) { // For each pixel...
    // pixels->Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    if ( a < numPixels )
    {
      pixels->setPixelColor(a, pixels->Color(50, 50, 0));
      pixels->setPixelColor(b, pixels->Color(50, 50, 0));
      pixels->show();   // Send the updated pixel colors to the hardware.
      delay(30);
    }
    pixels->setPixelColor(a, pixels->Color(0, 0, 0));
    pixels->setPixelColor(b, pixels->Color(0, 0, 0));
    pixels->show();
    delay(30);
    if ( a == 0 ) break;
  }
  pixels->clear();
}
void alarmRed()
{    
  for(int i = 0; i < 3; i++)
  {
    tone(buzzerPin, 2000); // Send sound signal...
    delay(300); 
   
    noTone(buzzerPin);     // Stop sound...
    allred();
    pixels->show();
    delay(100);
  
    tone(buzzerPin, 4000); // Send sound signal...
    delay(100);   
  
    noTone(buzzerPin);     // Stop sound...
    allred();
    pixels->show();
    delay(100); 

    alloff();
    pixels->show();
  }
  //delay(1000);
}
void allred()
{
  pixels->setPixelColor(0, pixels->Color(255, 0, 0));
  pixels->setPixelColor(1, pixels->Color(255, 0, 0));
  pixels->setPixelColor(2, pixels->Color(255, 0, 0));
  pixels->setPixelColor(3, pixels->Color(255, 0, 0));
  pixels->setPixelColor(4, pixels->Color(255, 0, 0));
  pixels->setPixelColor(5, pixels->Color(255, 0, 0));
  pixels->setPixelColor(6, pixels->Color(255, 0, 0));
  pixels->setPixelColor(7, pixels->Color(255, 0, 0));
}
void alloff()
{
  pixels->setPixelColor(0, pixels->Color(0, 0, 0));
  pixels->setPixelColor(1, pixels->Color(0, 0, 0));
  pixels->setPixelColor(2, pixels->Color(0, 0, 0));
  pixels->setPixelColor(3, pixels->Color(0, 0, 0));
  pixels->setPixelColor(4, pixels->Color(0, 0, 0));
  pixels->setPixelColor(5, pixels->Color(0, 0, 0));
  pixels->setPixelColor(6, pixels->Color(0, 0, 0));
  pixels->setPixelColor(7, pixels->Color(0, 0, 0));
}
void neoPercent(int gasValueMapped)
{
  int percentLED = 100 / numPixels;
  int pixelPercent = gasValueMapped / percentLED;

 //cycle up
  for(int i=0; i<pixelPercent; i++) { // For each pixel...
    pixels->setPixelColor(i, pixels->Color(0, 0, 50));
    pixels->show();   // Send the updated pixel colors to the hardware.
    delay(30);
  }
  delay(500);
  //cycle down
  for(int i=pixelPercent; i <= pixelPercent; i--) { // For each pixel...
    pixels->setPixelColor(i, pixels->Color(0, 0, 0));
    pixels->show();   // Send the updated pixel colors to the hardware.
    delay(30);
    if ( i == 0 ) break;
  }
  /* #DEBUG
  Serial.print("gasValueMapped: ");
  Serial.println(gasValueMapped);
  Serial.print("percentLED: ");
  Serial.println(percentLED);
  Serial.print("pixelPercent: ");
  Serial.println(pixelPercent);
  handleSerial();
  while(!Serial.available()); */
}
void blinkGreen() {
  pixels->setPixelColor(0, pixels->Color(0, 25, 0));
  pixels->setPixelColor(7, pixels->Color(0, 25, 0));
  pixels->show();
  delay(15);
  pixels->setPixelColor(0, pixels->Color(0, 0, 0));
  pixels->setPixelColor(7, pixels->Color(0, 0, 0));
  pixels->show();
}
