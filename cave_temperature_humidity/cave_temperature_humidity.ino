/* ------------------------------------------------------------------
|  MICROCONTROLER                                                   | 
|  Component: Arduino UNO - R3                                      |
|                                                                   |
|  SD CARD                                                          |
|  Component:   Pololu Electronics - Micro SD breakout with 3.3 V   |
|               resulator and level shifters.                       |
|               https://www.pololu.com/product/2587                 |
|  Connections: GND   ->  UNO GND                                   |
|               VDD   ->  UNO 5V                                    |
|               DI    ->  UNO PIN 11                                |
|               DO    ->  UNO PIN 12                                |
|               SCLK  ->  UNO PIN 13                                |
|               CS    ->  UNO PIN 4                                 | 
|                                                                   |
-------------------------------------------------------------------*/


#include <SPI.h>
#include <SD.h>

const int chipSelect_sd = 4;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  init_sd();
}

void loop() {  

  // Uncomment the following to test the SD Card
  // String fileName = "test.txt";
  // String data;
  // for (int y=0; y<4; y++) {
  //   data += String(random(0,100)) + ',';
  // }
  // write_sd(fileName, data);
}


void init_sd() {
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect_sd)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");
}

void write_sd(String file, String dataString) {
  File dataFile = SD.open(file, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString);
  } else {
    Serial.println("error opening " + file);
  }
}
