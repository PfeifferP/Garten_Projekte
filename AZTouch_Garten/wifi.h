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
