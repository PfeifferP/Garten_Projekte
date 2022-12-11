#include <Arduino.h>
#include <FS.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <TFT_eSPI.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <ESPConnect.h>
#include <LittleFS.h>

#include <Ticker.h>
#include <time.h>

#define NTP_SERVER "de.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#define LED0 2
#define BKLED 15
#define BEEPER 26

hw_timer_t * timer_clear_status = NULL;
time_t now;
tm tm;
String adate, atime;
String wochentage[7]={"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};

Ticker getWifiSignal;
Ticker pages;
Ticker refresh_page;

int actpage = 0, oldpage = 99;


TFT_eSPI tft=TFT_eSPI();
AsyncWebServer server(80);


/* --------------------------------------------------------- */
void IRAM_ATTR timer_clear_statusISR (){
  
  
  timerAlarmDisable(timer_clear_status);
}

void clear_top_bar(){
  tft.fillRect(0,0,320,15,TFT_LIGHTGREY);
  tft.drawRect(0,0,320,15,TFT_BLACK);
}

void clear_status_bar(){
  tft.fillRect(0,224,320,239,TFT_LIGHTGREY);
  tft.drawRect(0,224,320,239,TFT_BLACK);
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
  actpage++; if (actpage > 3) { actpage = 0; }
  
  Serial.println(String(actpage));
}
void getdatetime(){
  time(&now);
  localtime_r(&now, &tm);
  atime=""; adate="";
  adate = wochentage[tm.tm_wday] + ".  ";
  if (tm.tm_mday < 10) { adate += "0"; } 
  adate += tm.tm_mday; adate += ".";
  if (tm.tm_mon + 1 < 10){ adate +="0"; }
  adate += tm.tm_mon+1; adate += ".";
  adate += tm.tm_year + 1900;
  if (tm.tm_hour < 10) { atime = "0"; } 
  atime += tm.tm_hour; atime += ":";
  if (tm.tm_min < 10){ atime +="0"; }
  atime += tm.tm_min; atime += ":";
  if (tm.tm_sec < 10){ atime +="0"; }
  atime += tm.tm_sec;
}
  
void ref_page() {
  getdatetime();
  if (oldpage != actpage){
    tft.fillRect(0,15,320,209,TFT_WHITE);
    
    oldpage = actpage;
  }
  tft.drawString(adate,10,20,4);
  tft.drawString(atime,210,20,4);
  switch (actpage) {
  case 1:
    tft.setTextColor(TFT_GREEN, TFT_WHITE);
    break;
  case 2:
    tft.setTextColor(TFT_RED, TFT_WHITE);
    break;
  case 3:
    tft.setTextColor(TFT_BLUE, TFT_WHITE);
    break;
  default:
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    break;
  }
};

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  tft.drawCentreString(String(info.wifi_sta_disconnected.reason),160,120,2);
  Serial.println("Trying to Reconnect");
  //WiFi.begin(ssid, password);
}
/* --------------------------------------------------------- */


void setup() {
  //  disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG,0);
  // Interrupt Clear Status
  timer_clear_status = timerBegin(0,80,true);
  timerAttachInterrupt(timer_clear_status, timer_clear_statusISR, true);
  pinMode(BKLED, OUTPUT);
  digitalWrite(BKLED,0);
  Serial.begin(115200);
  Serial.println("Starting...");
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  clear_top_bar(); clear_status_bar();
  // starte NTP
  configTime(0,0,NTP_SERVER);
  setenv("TZ",MY_TZ,1);
  tzset();
  
  // starte Server
  ESPConnect.autoConnect("ESP32Config");
  ESPConnect.begin(&server);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  Serial.println("Formatting LittleFS filesystem");
  //LittleFS.format();
  Serial.println("Mount LittleFS");
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  server.addHandler(new SPIFFSEditor(LittleFS, "admin","admin"));
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");
  server.begin();
  drawWifiQuality();
  getWifiSignal.attach(60, drawWifiQuality);
  pages.attach(10,switch_pages);
  refresh_page.attach(1,ref_page);

  Serial.println(WiFi.localIP().toString()); 
  tft.setTextColor(TFT_BLACK,TFT_LIGHTGREY);
  tft.drawString(ESPConnect.getSSID(),10,5,1);
  tft.drawString(WiFi.localIP().toString(),100,5,1);
}

void loop() {
  // put your main code here, to run repeatedly:
}