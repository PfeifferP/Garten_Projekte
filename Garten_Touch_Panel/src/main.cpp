#include <Arduino.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <TFT_eSPI.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <ESPConnect.h>
#include <AsyncElegantOTA.h>

hw_timer_t * timer_clear_status = NULL;


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
  tft.fillRect(0,16,320,208,TFT_DARKGREY);
  
  // starte Server
  ESPConnect.autoConnect("ESP32Config");
  ESPConnect.begin(&server);
  AsyncElegantOTA.begin(&server);
  server.begin();

  Serial.println(WiFi.localIP().toString()); tft.drawString(WiFi.localIP().toString(),100,100,1);
}

void loop() {
  // put your main code here, to run repeatedly:
}