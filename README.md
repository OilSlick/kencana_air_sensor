# Air Sensor
An air sensor for inside a home. 

Currently I have the sensor deployed in my kitchen, near an LPG tank connected to the cooker. 

Update August 25, 2020: The device samples the air every twenty seconds as before, but now it maintains an average of these 20s samples for five minutes and then reports the five minute average to the web gateway to be further analyzed and ultimately sent to Adafruit's IO (AIO) platform. Previously the sensor sampled every 20s, and then every five minutes it sent a 'spot' sample to the gateway. Now the data sent to the gateway is more representative of the previous five minutes, not simply a spot-reading. 

- [ ] I need to improve the warning and alarm feature of the sensor

Some general information here: [http://blog.michaelcwright.com/2020/07/14/air-quality-sensor/](http://blog.michaelcwright.com/2020/07/14/air-quality-sensor/)

The AIO dashboard is here: https://io.adafruit.com/mcwresearch/dashboards/air-quality At the bottom of that dashboard, I also include some local weather conditions (updated every 10 minutes from OpenWeatherMap) as well as a link to a second dashboard to analyze NO2 data: https://io.adafruit.com/mcwresearch/dashboards/nitrogen-dioxide-no2-aq-stats

Keep in mind the base gas sensor from Seeed provides only a qualitative measurement. I have not calibrated it beyond what was done by the manufacturer. This is a hobby/experimental exercise, not a true environmental monitoring project. 
