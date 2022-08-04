// convert the dBm to Quality between 0 and 100%
int8_t getWifiQuality(){
  if (WiFi.status() != WL_CONNECTED){
    return -1;
  } else {
    int32_t dbm = WiFi.RSSI();
    if (dbm <= -100) {
      return 0;
    } else if (dbm >= -50) {
      return 100;
    } else {
      return (2 * (dbm + 100));
    }
  }
}

// draw Quality topline
void drawWifiQuality (){
  int8_t quality = getWifiQuality();
  tft.setTextColor(TFT_GREEN, TFT_LIGHTGREY);
  tft.drawRightString("  " + String(quality) + "%",305, 5, 1);
  for (int8_t i=0; i<4; i++){
    tft.drawFastVLine(310 + 2 * i, 4, 8, TFT_LIGHTGREY);
    for (int8_t j=0; j<2*(i+1); j++){
      if (quality > i*25 || j == 0){
        tft.drawPixel(310 + 2*i, 12-j, TFT_GREEN);
      }
    }
  }
}
// ---- init WiFi
bool initWiFi(){
  if (String(mySet.ssid)==""){
    setStatusBar("Keine SSID");
    return false;
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(mySet.ssid.c_str(), mySet.wkey.c_str());
  setStatusBar("Connecting to WiFi");
  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  while(WiFi.status() != WL_CONNECTED){
    currentMillis = millis();
    if (currentMillis - previousMillis >= (DEFAULT_WIFI_TIMEOUT * 1000)){
      setStatusBar("failed to connect.");
      return false;
    }
  }
  setStatusBar(WiFi.localIP().toString().c_str());
  return true;
}
