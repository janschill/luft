/*

1. Read TVOC and eCO2 from SPG30
2. Determine if air quality is good or bad
3. Print
  3a. Support multi language

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
#include "SparkFun_SGP30_Arduino_Library.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// 4-pin
#define OLED_RESET 4 // RES
#define SCREEN_ADDRESS 0x3C // 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 7-pin
// #define OLED_MOSI 2 // D1
// #define OLED_CLK 3 // D0
// #define OLED_DC 4 // DC
// #define OLED_CS 5 // CS
// #define OLED_RESET 6 // RES
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI,
OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

SGP30 sgp30;
SGP30ERR error;

void setup() {
  // Two LEDs
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  Serial.begin(9600);
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  // if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

  if (sgp30.begin() == false) {
    Serial.println("No SGP30 Detected. Check connections.");
    printWarning('A');
    while (1);
  }
  sgp30.initAirQuality();
}

int prevEco2 = 0;
int prevTvoc = 0;

void loop() {
  error = sgp30.measureAirQuality();
  if (error == SGP30_SUCCESS) {
    int eco2 = sgp30.CO2;
    int tvoc = sgp30.TVOC;

    if (!isBaseValues(eco2, tvoc)) {
      if (prevEco2 != eco2 || prevTvoc != tvoc) {
        bool isGoodAir = isGoodAirQuality(eco2, tvoc);
        printSuccess(eco2, tvoc, isGoodAir);
        blink(isGoodAir);
        prevEco2 = eco2;
        prevTvoc = tvoc;
      }
      delay(9000);
    } else {
      printWarning('B');
      blink(true);
    }
  }
  delay(1000);
}

void blink(bool isGoodAir) {
  if (isGoodAir) {
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
  } else {
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
  }
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
    default:
      display.println("Error");
      break;
  }
}

void printSuccess(int eco2, int tvoc, bool isGoodAir) {
  display.setTextColor(WHITE);

  printFirstRow('A', isGoodAir);
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
  display.clearDisplay();
}

void printWarning(unsigned char code) {
  display.setTextColor(WHITE);
  display.setCursor(4, 4);
  display.setTextSize(1);

  switch (code) {
    case 'A':
      display.println("Sensor not found.");
      display.println("Trying again ...");
      break;
    case 'B':
      display.println("Sensor calibrating.");
      break;
    case 'C':
      display.println("Sensor reading error.");
      display.println("Trying again ...");
      break;
    default:
      display.println("Unknown warning");
      break;
  }

  display.display();
  display.clearDisplay();
}

bool isGoodAirQuality(int eco2, int tvoc) {
  return isGoodECO2(eco2) && isGoodTVOC(tvoc);
}

bool isGoodECO2(int eco2) {
  return 0 <= eco2 && eco2 < 750;
}

bool isGoodTVOC(int tvoc) {
  return 0 <= tvoc && tvoc < 150;
}

bool isBaseValues(int eco2, int tvoc) {
  return eco2 == 400 && tvoc == 0;
}
