/* ------------------------------------------------------------------
|  MICROCONTROLER                                                   | 
|  Component: Arduino UNO - R3                                      |
|                                                                   |
|  SD CARD                                                          |
|  Component:   Pololu Electronics - Micro SD breakout with 3.3 V   |
|               regulator and level shifters.                       |
|               https://www.pololu.com/product/2587                 |
|  Connections: GND   ->  UNO GND                                   |
|               VDD   ->  UNO 5V                                    |
|               DI    ->  UNO PIN 11                                |
|               DO    ->  UNO PIN 12                                |
|               SCLK  ->  UNO PIN 13                                |
|               CS    ->  UNO PIN 4                                 | 
|                                                                   |
|  TEMPERATURE                                                      |
|  Component:   18B20 Digital Temperature Sensor                    |
|               https://www.robotics.org.za/18B20-1M                |
|  Connections: GND   ->  UNO GND                                   |
|               VDD   ->  UNO 5V                                    |
|               DATA  ->  UNO PIN 2                                 |
|               4K7 Resistor VDD -> DATA                            | 
|                                                                   |
|  TIMER                                                            |
|  Component:   Chronodot Real Time Clock                           |
|               https://docs.macetech.com/doku.php/chronodot_v3.0   |
|  Connections: GND   ->  UNO GND                                   |
|               VCC   ->  UNO 5V                                    |
|               SCL   ->  UNO A5                                    |
|               SDA   ->  UNO A4                                    |
|                                                                   |
-------------------------------------------------------------------*/


#include <SPI.h>
#include <SD.h>
#include <OneWire.h>
#include <RTClib.h>

const int chipSelect_sd = 4;
#define ONE_WIRE_BUS 8

OneWire oneWire(ONE_WIRE_BUS);
RTC_DS3231 rtc;

float temperature;
String filename;
String log_line;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  init_sd();
  init_timer();
}

void loop() {  
    delay(2000);
    filename = get_file_name();
    Serial.println(filename);
    temperature  = temperature_18B20_read();    

    DateTime now = rtc.now();
    log_line =  now.timestamp() + ", " + String(temperature);
    write_sd(filename, log_line);
}

String get_file_name() {
  DateTime now = rtc.now();
  filename = now.timestamp().substring(0, 10);
  filename.replace("-", "");
  return filename + ".log";
}

void init_sd() {
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect_sd)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized");
}

void init_timer() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find the RTC module!");
    while (1);
  }
 
  if (rtc.lostPower()) {
    Serial.println("RTC lost power. Setting the time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void write_sd(String file, String dataString) {
  File dataFile = SD.open(file, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening " + file);
  }
}

float temperature_18B20_read() {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
 
  if (!oneWire.search(addr)) {
    oneWire.reset_search();
    delay(250);
    return;
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return;
  }
   
  switch (addr[0]) {
    case 0x10:
      type_s = 1;
      break;
    case 0x28:
      type_s = 0;
      break;
    case 0x22:
      type_s = 0;
      break;
    default:
      Serial.println("Device is not recognized!");
      return;
  }

  oneWire.reset();
  oneWire.select(addr);
  oneWire.write(0x44, 1);
 
  // wait for conversion to complete
  delay(1000);

  oneWire.reset();
  oneWire.select(addr);
  oneWire.write(0xBE);
  for (i = 0; i < 9; i++) {
    data[i] = oneWire.read();
  }
  int16_t raw = (data[1] << 8) | data[0];
  float celsius = (float)raw / 16.0;
  return celsius;
}
