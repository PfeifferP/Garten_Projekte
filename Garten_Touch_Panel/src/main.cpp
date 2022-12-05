#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <TFT_eSPI.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <ESPConnect.h>
#include <AsyncElegantOTA.h>
#include <Ticker.h>

hw_timer_t * timer_clear_status = NULL;
Ticker getWifiSignal;
Ticker pages;

TFT_eSPI tft=TFT_eSPI();
AsyncWebServer server(80);


/* --------------------------------------------------------- */
void IRAM_ATTR timer_clear_statusISR (){
  
  
  timerAlarmDisable(timer_clear_status);
}

void clear_top_bar(){
  tft.fillRoundRect(0,0,320,16,1,TFT_LIGHTGREY);
  tft.drawRoundRect(0,0,320,16,1,TFT_GREEN);
}

void clear_status_bar(){
  tft.fillRoundRect(0,224,320,240,1,TFT_LIGHTGREY);
  tft.drawRoundRect(0,224,320,240,1,TFT_GREEN);
}

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  if (WiFi.status() != WL_CONNECTED){
    return -1;
  } else {
    int32_t dbm = WiFi.RSSI();
    if(dbm <= -100) {
        return 0;
    } else if(dbm >= -50) {
        return 100;
    } else {
        return 2 * (dbm + 100);
    }
  }
}

void drawWifiQuality() {
  int8_t quality = getWifiQuality();
  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  tft.drawRightString("  " + String(quality) + "%",305, 5, 1);
  for (int8_t i = 0; i < 4; i++) {
    tft.drawFastVLine(310 + 2 * i,4,8, TFT_LIGHTGREY);
    for (int8_t j = 0; j < 2 * (i + 1); j++) {
      if (quality > i * 25 || j == 0) {
        tft.drawPixel(310 + 2 * i, 12 - j,TFT_BLACK);
      }
    }
  }
}

void switch_pages(){

}
/* --------------------------------------------------------- */


void setup() {
  //  disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG,0);
  // Interrupt Clear Status
  timer_clear_status = timerBegin(0,80,true);
  timerAttachInterrupt(timer_clear_status, timer_clear_statusISR, true);


  Serial.begin(115200);
  Serial.println("Starting...");
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  clear_top_bar(); clear_status_bar();
  
  
  // starte Server
  ESPConnect.autoConnect("ESP32Config");
  ESPConnect.begin(&server);
  AsyncElegantOTA.begin(&server);
  server.begin();
  drawWifiQuality();
  getWifiSignal.attach(60, drawWifiQuality);
  pages.attach(120,switch_pages);
  
  Serial.println(WiFi.localIP().toString()); 
  tft.setTextColor(TFT_BLACK,TFT_LIGHTGREY);
  tft.drawString(ESPConnect.getSSID(),10,5,1);
  tft.drawString(WiFi.localIP().toString(),100,5,1);
}

void loop() {
  // put your main code here, to run repeatedly:
}