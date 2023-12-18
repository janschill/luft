/*

1. Read TVOC and eCO2 from SPG30
2. Determine if air quality is good or bad
3. Print
  3a. Support multi language
  3b. 

SGP30
TVOC (Total Volatile Organic Compound)
  concentration within a range of 0 to 60,000 parts per billion (ppb)
eCO2 (equivalent calculated carbon-dioxide)
  concentration within a range of 400 to 60,000 parts per million (ppm)
---

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI 9 // D1
#define OLED_CLK 10 // D0
#define OLED_DC 11 // DC
#define OLED_CS 12 // CS
#define OLED_RESET 13 // RES

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void setup() {
  Serial.println("Setting up sketch");
  pinMode(7, OUTPUT);

  int eco2 = readSGP30(0);
  int tvoc = readSGP30(1);
  bool isGoodAir = isGoodAirQuality(eco2, tvoc);

  print(eco2, tvoc, isGoodAir);
  blink(isGoodAir);
}

void blink(bool isGoodAir) {
  if (!isGoodAir) {
    digitalWrite(7, HIGH);
  }
}

void loop() {}

int readSGP30(int mode) {
  if (mode == 0) {
    return 701;
  }
  if (mode == 1) {
    return 320;
  }
  return 0;
}

bool isGoodAirQuality(int eco2, int tvoc) {
  return isGoodECO2(eco2) && isGoodTVOC(tvoc);
}

bool isGoodECO2(int eco2) {
  return 0 < eco2 && eco2 < 700;
}

bool isGoodTVOC(int tvoc) {
  return 0 < tvoc && tvoc < 700;
}

void iAmNotFeelingWell() {

}

void printFirstRow(unsigned char lang, bool isGoodAir) {
  display.setCursor(4, 4);
  display.setTextSize(2);

  switch (lang) {
    case 'A':
      if (isGoodAir) {
        display.println("God luft");
      } else {
        display.println("Luft ut");
      }
      break;
    case 'B':
      if (isGoodAir) {
        display.println("Gute Luft!");
      } else {
        display.println("Stossluft!");
      } 
      break;
    case 'C':
      // 
      break;
    case 'D':
      // 
      break;
    default:
      iAmNotFeelingWell();
      break;
  }
}

void print(int eco2, int tvoc, bool isGoodAir) {

  display.begin(SSD1306_SWITCHCAPVCC); 
  display.clearDisplay();
  display.display();     
  display.setTextColor(WHITE);

  printFirstRow('B', isGoodAir);
  display.drawFastHLine(0, 24, 128, WHITE);
 
  display.setTextSize(2);
  display.setCursor(4, 30);
  display.print("VOC:");
  display.print(tvoc);
  display.setTextSize(1);
  display.println("ppb");
  display.setTextSize(2);
  display.setCursor(4, 50);
  display.print("CO2:");
  display.print(eco2);
  display.setTextSize(1);
  display.println("ppm");
  display.display();
}