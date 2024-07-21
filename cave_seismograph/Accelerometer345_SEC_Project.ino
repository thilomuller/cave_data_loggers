/* ------------------------------------------------------------------
|  MICROCONTROLER                                                   | 
|  Component: Arduino Nano - R3                                     |
|                                                                   |
|  SD CARD                                                          |
|  Component:   MicroSD card module 6-pin                           |
|  Connections: GND   ->  NANO GND                                  |
|               3V3   ->  NANO 3V3                                  |
|               CS    ->  NANO PIN 4                                |
|               MOSI  ->  NANO PIN 11                               |
|               CLK   ->  NANO PIN 13                               |
|               MISO  ->  NANO PIN 12                               | 
|                                                                   |
|  ACCELEROMETER                                                    |
|  Component:   Triple axis accelerometer ADXL345 Module GY-291     |
|               www.diyelectronics.co.za                            |
|  Connections: GND   ->  NANO GND                                  |
|               VCC   ->  NANO 3V3                                  |
|               SDA   ->  NANO PIN A4                               |
|               SCL   ->  NANO PIN A5                               | 
|                                                                   |
|  TIME                                                             |
|  Component:   Chronodot V2.1 Real Time Clock                      |
|               https://docs.macetech.com/doku.php/chronodot_v3.0   |
|  Connections: GND   ->  UNO GND                                   |
|               VCC   ->  UNO 5V                                    |
|               SCL   ->  UNO A5                                    |
|               SDA   ->  UNO A4                                    |
|                                                                   |
|..................................................................*/

#include "RTClib.h"       //Realtime clock library(RTC)
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <SPI.h>          //Load SPI communication library**
#include <SD.h>           //Load the SD library**

RTC_DS3231 rtc;           //Add Realtime clock object (RTC)
int chipSelect=4;         //Variables for micro SD (set chipSelect = 4)**
File mySensorData;        //Variable for working with micro SD file object**

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void displaySensorDetails(void)
{
 sensor_t sensor;
 accel.getSensor(&sensor);
 Serial.println("------------------------------------");
 Serial.print ("Sensor: "); Serial.println(sensor.name);
 Serial.print ("Driver Ver: "); Serial.println(sensor.version);
 Serial.print ("Unique ID: "); Serial.println(sensor.sensor_id);
 Serial.print ("Max Value: "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
 Serial.print ("Min Value: "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
 Serial.print ("Resolution: "); Serial.print(sensor.resolution); Serial.println(" m/s^2"); 
 Serial.println("------------------------------------");
 Serial.println("");
 delay(500);
}

void displayDataRate(void)
{
 Serial.print ("Data Rate: "); 
 
 switch(accel.getDataRate())
 {
 case ADXL345_DATARATE_3200_HZ:
 Serial.print ("3200 "); 
 break;
 case ADXL345_DATARATE_1600_HZ:
 Serial.print ("1600 "); 
 break;
 case ADXL345_DATARATE_800_HZ:
 Serial.print ("800 "); 
 break;
 case ADXL345_DATARATE_400_HZ:
 Serial.print ("400 "); 
 break;
 case ADXL345_DATARATE_200_HZ:
 Serial.print ("200 "); 
 break;
 case ADXL345_DATARATE_100_HZ:
 Serial.print ("100 "); 
 break;
 case ADXL345_DATARATE_50_HZ:
 Serial.print ("50 "); 
 break;
 case ADXL345_DATARATE_25_HZ:
 Serial.print ("25 "); 
 break;
 case ADXL345_DATARATE_12_5_HZ:
 Serial.print ("12.5 "); 
 break;
 case ADXL345_DATARATE_6_25HZ:
 Serial.print ("6.25 "); 
 break;
 case ADXL345_DATARATE_3_13_HZ:
 Serial.print ("3.13 "); 
 break;
 case ADXL345_DATARATE_1_56_HZ:
 Serial.print ("1.56 "); 
 break;
 case ADXL345_DATARATE_0_78_HZ:
 Serial.print ("0.78 "); 
 break;
 case ADXL345_DATARATE_0_39_HZ:
 Serial.print ("0.39 "); 
 break;
 case ADXL345_DATARATE_0_20_HZ:
 Serial.print ("0.20 "); 
 break;
 case ADXL345_DATARATE_0_10_HZ:
 Serial.print ("0.10 "); 
 break;
 default:
 Serial.print ("???? "); 
 break;
 } 
 Serial.println(" Hz"); 
}

void displayRange(void)
{
 Serial.print ("Range: +/- "); 
 
 switch(accel.getRange())
 {
 case ADXL345_RANGE_16_G:
 Serial.print ("16 "); 
 break;
 case ADXL345_RANGE_8_G:
 Serial.print ("8 "); 
 break;
 case ADXL345_RANGE_4_G:
 Serial.print ("4 "); 
 break;
 case ADXL345_RANGE_2_G:
 Serial.print ("2 "); 
 break;
 default:
 Serial.print ("?? "); 
 break;
 } 
 Serial.println(" g"); 
}

void setup(void) 
{
 Serial.begin(9600);
 Wire.begin();              //For RTC*
 Serial.println("Accelerometer Test"); Serial.println("");
 pinMode(10,OUTPUT);// Reserve pin 10 for SD**
 SD.begin(chipSelect);//Initialize the SD card with chipSelect (CS) on pin 4**
 
 /* Initialise the sensor */
 if(!accel.begin())
 {
 /* There was a problem detecting the ADXL345 ... check your connections */
 Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
 while(1);
 }

 /* Set the range to whatever is appropriate for your project */
 accel.setRange(ADXL345_RANGE_16_G);
 // displaySetRange(ADXL345_RANGE_8_G);
 // displaySetRange(ADXL345_RANGE_4_G);
 // displaySetRange(ADXL345_RANGE_2_G);
 
 /* Display some basic information on this sensor */
 displaySensorDetails();
 
 /* Display additional settings (outside the scope of sensor_t) */
 displayDataRate();
 displayRange();
 Serial.println("");

   // Initialize the DS3231 RTC module
  if (!rtc.begin()) {
    Serial.println("Couldn't find the RTC module!");
    while (1);
  }
 
  // Check if the RTC module lost power
  if (rtc.lostPower()) {
    Serial.println("RTC lost power. Setting the time...");
    // Set the RTC module to the current date and time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop(void) {

  // Read the current date and time from the DS3231 RTC module
  DateTime now = rtc.now();
 
  // Print the date and time to the Serial Monitor
  Serial.print("Date: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" Time: ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
 
  delay(50);

 /* Get a new sensor event */ 
 sensors_event_t event; 
 accel.getEvent(&event);

mySensorData=SD.open("secpro.txt", FILE_WRITE);     //Open file "acc.txt" on the SD card as a file to write to
if (mySensorData) {

 /* Display the results (acceleration is measured in m/s^2) */
 Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print(" ");
 Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print(" ");
 Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print(" ");Serial.println("m/s^2 ");
 delay(50);

 mySensorData.print(event.acceleration.x);
 mySensorData.print(",");
 mySensorData.print(event.acceleration.y);
 mySensorData.print(",");
 mySensorData.println(event.acceleration.z);
 mySensorData.print("Date: ");
 mySensorData.print(now.year(), DEC);
 mySensorData.print('/');
 mySensorData.print(now.month(), DEC);
 mySensorData.print('/');
 mySensorData.print(now.day(), DEC);
 mySensorData.print(" Time: ");
 mySensorData.print(now.hour(), DEC);
 mySensorData.print(':');
 mySensorData.print(now.minute(), DEC);
 mySensorData.print(':');
 mySensorData.print(now.second(), DEC);
 mySensorData.println();
 mySensorData.close();
}
}