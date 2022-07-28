// --- Directory listing ---
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

// --- Initialize WiFi ---
bool initWiFi() {
  if(String(settings.ssid)==""){
    Serial.println("Undefined SSID or IP address.");
    return false;
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(settings.ssid, settings.wkey);
  Serial.println("Connecting to WiFi...");
  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  while(WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("Failed to connect.");
      return false;
    }
  }
  Serial.println(WiFi.localIP());
  return true;
}

void sound(int freq) {
  ledcWriteTone(1,freq);
}

// --- init Touch mit TFT_CAL_FILE
void initTouch(){
  uint16_t calData[5];
  uint8_t calDataOK = 0;
  // check if calibration file exists and size is correct
  if (LittleFS.exists(TFT_CAL_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      LittleFS.remove(TFT_CAL_FILE);
    }
    else
    {
      File f = LittleFS.open(TFT_CAL_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }
  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // Calibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  
    tft.println("Touch corners as indicated");
  
    tft.setTextFont(1);
    tft.println();
    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }
    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = LittleFS.open(TFT_CAL_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
  
}

void clearStatusBar(){
  tft.fillRoundRect(0, 224, 320, 240, 1, TFT_LIGHTGREY);
  tft.drawRoundRect(0,224,320,240,1,TFT_BLACK);
}

void setStatusBar(String text){
  clearStatusBar;
  tft.setTextColor(TFT_BLACK,TFT_LIGHTGREY);
  tft.drawString(text,10,229,1);
  timerAlarmWrite(timer1,10000000,false);
  timerAlarmEnable(timer1);
  tft.setTextColor(TFT_BLACK,TFT_WHITE);
}

void IRAM_ATTR timer1ISR(){
  clearStatusBar();
  timerAlarmDisable(timer1);
}

// Load the configuration from a file
void loadSettings(Settings &settings) {
  File file = LittleFS.open(CONFIG_FILE);
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error){
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
  }
  settings.hostname = doc["hostname"]| "myhostname"; 
  settings.port = doc["port"] | 80; 
  settings.ssid = doc["wifi"]["ssid"] | "mess"; 
  settings.wkey = doc["wifi"]["pass"] | "Dreebit2016"; 

  settings.http_user = doc["edit"]["user"] | "admin"; 
  settings.http_pass = doc["edit"]["pass"] | "admin"; 
  file.close();
}

// Saves the configuration to a file
void saveConfiguration(const Settings &settings) {
  LittleFS.remove(CONFIG_FILE);
  File file = LittleFS.open(CONFIG_FILE, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<512> doc;
  doc["hostname"] = settings.hostname;
  doc["port"] = settings.port;
  JsonObject wifi = doc.createNestedObject("wifi");
    wifi["ssid"] = settings.ssid;
    wifi["pass"] = settings.wkey;

  JsonObject edit = doc.createNestedObject("edit");
    edit["user"] = settings.http_user;
    edit["pass"] = settings.http_pass;
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  serializeJsonPretty(doc, Serial);
  file.close();
}
