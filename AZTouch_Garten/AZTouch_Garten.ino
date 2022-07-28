
#include <FS.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FSEditor.h>
#include <AsyncElegantOTA.h>
#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
AsyncWebServer server(80);
//------------------------------------------------------------------------------------------
#define BKLED 15
#define BEEPER 26
#define TFT_CAL_FILE "/.touchdata"
#define CONFIG_FILE "/settings.json"
#define REPEAT_CAL false
#define FORMAT_LITTLEFS_IF_FAILED true
struct Settings {
  const char* hostname;
  const int port;
  const char* ssid;
  const char* wkey;
  const char* http_user;
  const char* http_pass;
};

Settings settings;
// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)
// ISR Timer
hw_timer_t * timer1 = NULL;
//------------------------------------------------------------------------------------------
#include "subs.h"
#include "wifi.h"
#include "cron.h"

void setup() {
  // Use serial port
  Serial.begin(115200);
  // set Interrupt Timer1
  timer1 = timerBegin(0,80,true);
  timerAttachInterrupt(timer1, &timer1ISR, true);
  loadSettings(settings);
  // Setup Backlight
  ledcSetup(0, 5000, 8); // 0-15, 5000, 8
  ledcAttachPin(BKLED, 0); // TFT_BL, 0 - 15
  ledcWrite(0,0);
  // Setup Beeper
  ledcSetup(1,8000,12);
  ledcAttachPin(BEEPER,1);
  // starte Filesystem
  if (!LittleFS.begin(false,"")) {
    Serial.println("Formatting file system");
    LittleFS.format();
    LittleFS.begin(false,"");
  }
  Serial.println("Informationen zum Dateisystem:");
  Serial.printf("- Bytes total:   %ld\n", LittleFS.totalBytes());
  Serial.printf("- Bytes genutzt: %ld\n\n", LittleFS.usedBytes());
  listDir(LittleFS, "/", 3);
  // Initialisiere WiFi
  initWiFi();
  // Initialise the TFT screen
  tft.init();
  tft.setRotation(1);
  initTouch();        //init Touch
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK,TFT_WHITE);
  // Draw TopBar
  tft.fillRoundRect(0, 0, 320, 16, 1, TFT_LIGHTGREY);
  tft.drawRoundRect(0,0,320,16,1,TFT_BLACK);
  // Draw StatusBar
  clearStatusBar();


  
  tft.drawCentreString("Touch screen to test!",tft.width()/2, tft.height()/2, 2);
  saveConfiguration(settings);
  setStatusBar("Save settings. ");
  listDir(LittleFS, "/", 3);
  server.addHandler(new FSEditor(LittleFS, http_user,http_pass));

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm");

  AsyncElegantOTA.begin(&server);
  server.begin();
  
}

//------------------------------------------------------------------------------------------

void loop(void) {
  uint16_t x = 0, y = 0; // To store the touch coordinates

  // Pressed will be set true is there is a valid touch on the screen
  bool pressed = tft.getTouch(&x, &y);

  // Draw a white spot at the detected coordinates
  if (pressed) {
    tft.fillCircle(x, y, 2, TFT_WHITE);
    //Serial.print("x,y = ");
    //Serial.print(x);
    //Serial.print(",");
    //Serial.println(y);
  }
  
  cronjob();
}

//------------------------------------------------------------------------------------------
