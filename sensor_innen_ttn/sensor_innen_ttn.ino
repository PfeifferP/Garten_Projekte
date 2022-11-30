/*
 *  https://wiki.octoate.de/doku.php/thethingsnetwork:esp32_mit_868_mhz_lora_modul 
 *  https://www.aeq-web.com/starthilfe-lorawan/
 *  https://unsinnsbasis.de/esp32-preferences/
 * XXxxaa / UAdvmoa7w:~
 */
#include <Wire.h>
 
// WiFi
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>


//OLED 
#include <U8x8lib.h>
// Sensor
#include <Adafruit_BME680.h>
// LMIC
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

// Config Speicher Parameter
#include <Preferences.h>
// Timer
#include <Ticker.h>
// addons
#include <PubSubClient.h>

// Definierungen
#include "defs.h"
#include "lora_data.h"
#include "funktionen.h"
#include "lora_func.h"
#include "sensor.h"
#include "prefs.h"
/* ------------------------------------------*/

void setup() {
    Serial.begin(115200);
    Serial.println(F("Starting"));
    
    display.begin();
    display.setPowerSave(0);
    display.setFont(u8x8_font_chroma48medium8_r);
    setup_sensor();
    setup_config();
   
    display.setCursor(0,5);
    display.print("Start-ups: ");
    display.print(counter);
    
    SPI.begin(5, 19, 27, 18);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while ((WiFi.waitForConnectResult() != WL_CONNECTED) && (counts > 0)) {
      Serial.println("Connection Failed! Rebooting...");
      delay(1000);
      counts--;
      //ESP.restart();
    }
    
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);

    getSensorData.attach(60, getBME680Readings);
}

void loop() {
    os_runloop_once();
    if (!client.connected()) {
      reconnect();
    }
    client.loop();    
}
