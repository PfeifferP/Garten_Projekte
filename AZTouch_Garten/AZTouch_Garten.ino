/* -------------------------------------------------------
 *  https://unsinnsbasis.de/esp32-preferences/
 *  
 *  https://microcontrollerslab.com/esp32-wi-fi-manager-asyncwebserver/
 *  
 */
#include <WiFi.h>
#include <Preferences.h>
#include <time.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

Preferences config;

#include "config.h"
#include "functions.h"

void setup() {
  bool f_error = false; // Fehler-Flag
  String t_error = ""; // Fehlertext
  Serial.begin(115200);
  config.begin(wifi_namespace, true);
  size_t whatsLeft = config.freeEntries();    
  Serial.printf("%u - Hier sind: %u Einträge verfügbar.\n", wifi_namespace, whatsLeft);
  config.end();
  config.begin(boot_namespace, true);
  whatsLeft = config.freeEntries();    
  Serial.printf("%u - Hier sind: %u Einträge verfügbar.\n", boot_namespace, whatsLeft);
  config.end();
  
}



void loop() {

}
