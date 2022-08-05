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
   if(initWiFi()) {
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, "/index.html", "text/html");
    });
    server.serveStatic("/", LittleFS, "/");
    server.begin();
  } 
  else {
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("ESP-WIFI-MANAGER", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/wifimanager.html", "text/html");
    });
    server.serveStatic("/", SPIFFS, "/");
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == "ssid") {
            mySet.ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(mySet.ssid);
          }
          // HTTP POST pass value
          if (p->name() == "pass") {
            mySet.wkey = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(mySet.wkey);
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart.");
      delay(3000);
      ESP.restart();
    });
    server.begin();
  }

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
