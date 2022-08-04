
void loadConfig(){
  conf.begin("configs", false);
  bool doesExist = conf.isKey("isinit"); 
  mySet.ssid = conf.getString("ssid", "MikroTik-220CF6");
  mySet.wkey = conf.getString("wkey", "Sanifar123!");
  mySet.ntps = conf.getString("ntpserver", DEFAULT_NTP_SERVER);
  conf.end();
}

void clearStatusBar(){
  tft.fillRoundRect(0, 224, 320, 240, 1, TFT_LIGHTGREY);
  tft.drawRoundRect(0, 224, 320, 240, 1, TFT_BLACK);
  t_error = "";
  f_error = false;
}

void setStatusBar(String text){
  clearStatusBar();
  tft.setTextColor(TFT_DARKGREEN,TFT_LIGHTGREY);
  tft.drawString(text, 10, 229, 1);
  timerAlarmWrite(tim1, 10000000, false);
  timerAlarmEnable(tim1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

void sound(int freq, int dauer = 10){
  ledcWriteTone(1, freq);
  timerAlarmWrite(tim2, (100000 * dauer), false);
  timerAlarmEnable(tim2);
}

void IRAM_ATTR tim1ISR(){
  clearStatusBar();
  timerAlarmDisable(tim1);
}

void IRAM_ATTR tim2ISR(){
  ledcWriteTone(1,0);
  timerAlarmDisable(tim2);
}

void initTouch(){
  uint16_t calData[5];
  uint8_t  calDataOK = 0;
  if (LittleFS.exists(TFT_CAL_FILE)){
    if (REPEAT_CAL){
      LittleFS.remove(TFT_CAL_FILE);
    } else {
      File f = LittleFS.open(TFT_CAL_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData,14) == 14) calDataOK =1;
        f.close();      
      }
    }
  if (calDataOK && !REPEAT_CAL){
    tft.setTouch(calData);
  } else {
    // Calibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20,0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.println("berühre die Ecken");
    tft.setTextFont(1);
    tft.println();
    tft.calibrateTouch(calData, TFT_YELLOW, TFT_BLACK, 15);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Cal OK");
    // store Data
    File f = LittleFS.open(TFT_CAL_FILE, "w");
    if (f){
      f.write((const unsigned char *)calData,14);
      f.close();
    }
  }
}
}
