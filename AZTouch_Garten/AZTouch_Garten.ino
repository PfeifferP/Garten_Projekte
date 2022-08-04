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
#include "wifi.h"
#include "cron.h"
/* -------------------------------------------------------- */
void setup() {

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
// set TFT
  tft.init();
  tft.setRotation(1);
  //sound(1000);
  initTouch();
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.fillRoundRect(0, 0, 320, 16, 1, TFT_LIGHTGREY);
  tft.drawRoundRect(0, 0, 320, 16, 1, TFT_BLACK);
  clearStatusBar();
// set WiFi
   initWiFi();

  //setStatusBar("init perfekt!");
  //server.begin();
  

  int xpos = tft.width() / 2; // Half the screen width
  int ypos = tft.height() / 2;
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawCentreString("20:33", xpos, ypos, 8);
}



void loop() {
  uint16_t x = 0, y = 0;
  bool tpressed = tft.getTouch(&x, &y);
  if (tpressed){
    tft.fillCircle(x,y,2,TFT_WHITE);
  }
  cronjob();
}
