#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid     = "MY_WLAN";
const char* password = "WLAN_PASSWORT";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;  
const int   daylightOffset_sec = 3600;

unsigned long lastDisplayUpdate = 0;

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("OLED not found");
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.println("Connecting");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  if (millis() - lastDisplayUpdate >= 1000) {
    lastDisplayUpdate = millis();
    updateDisplay();
  }
}

void updateDisplay() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Synch..");
    return;
  }

  display.clearDisplay();
  
  display.setTextSize(2);
  display.setCursor(15, 25);
  display.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  
  display.setTextSize(1);
  display.setCursor(35, 50);
  display.printf("%02d.%02d.%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  
  display.display();
}
