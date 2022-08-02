
void loadConfig(){
  conf.begin("configs", false);
  bool doesExist = conf.isKey("isinit"); 
  mySet.ssid = conf.getString("ssid", "");
  mySet.wkey = conf.getString("wkey", "");
  mySet.ntps = conf.getString("ntpserver", DEFAULT_NTP_SERVER);
  conf.end();
}

void clearStatusBar(){
  tft.fillRoundRect(0, 224, 320, 240, 1, TFT_LIGHTGREY);
  tft.drawRoundRect(0, 224, 320, 240, 1, TFT_BLACK);
}

void setStatusBar(String text){
  clearStatusBar();
  tft.setTextColor(TFT_BLACK,TFT_LIGHTGREY);
  tft.drawString(text, 10, 229, 1);
  timerAlarmWrite(tim1, 10000000, false);
  timerAlarmEnable(tim1);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
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
