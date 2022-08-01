#include <FS.h>
#include <LittleFS.h>
#include <Preferences.h>

#include <WiFi.h>
#include <time.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

#include <SPI.h>
#include <TFT_eSPI.h>
/* -------------------------------------------------------- */
TFT_eSPI tft = TFT_eSPI();
AsyncWebServer server(80);
Preferences conf;
/* -------------------------------------------------------- */
#include "config.h"
#include "functions.h"
/* -------------------------------------------------------- */
void setup() {
  bool f_error = false; // Fehler-Flag
  String t_error = ""; // Fehlertext
  struct settings mySet;
  Serial.begin(115200);
// set blank interrupt statusline
  tim1 = timerBegin(0,80,true);
  timerAttachInterrupt(tim1, &tim1ISR, true);
// set sound interrupt 
  tim2 = timerBegin(1,80,true);
  timerAttachInterrupt(tim2, &tim2ISR, true);
// set backlight
  ledcSetup(0, 5000, 8);
  ledcAttachPin(BKLED,0);
  ledcWrite(0, 0);
// set Beeper
  ledcSetup(1, 8000, 12);
  ledcAttachPin(BEEPER, 1);
// set filesystem
  if(!LittleFS.begin(false, "")) {
    Serial.println("Formatiere Filesystem!");
    LittleFS.format();
    LittleFS.begin(false, "");
  }
// load Config
  loadConfig();
  Serial.println(ssid);
  Serial.println(wkey);
  Serial.println(ntps);
// set WiFi
  // initWiFi();
// set TFT
  tft.init();
  tft.setRotation(1);
  //sound(1000);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.fillRoundRect(0, 0, 320, 16, 1, TFT_LIGHTGREY);
  tft.drawRoundRect(0, 0, 320, 16, 1, TFT_BLACK);
  clearStatusBar();
  setStatusBar("init perfekt!");
  //server.begin();
}



void loop() {

}
