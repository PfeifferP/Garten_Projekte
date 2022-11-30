/*
 * https://wiki.octoate.de/doku.php/thethingsnetwork:esp32_mit_868_mhz_lora_modul 
 *  https://www.aeq-web.com/starthilfe-lorawan/
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

// storage Parameter
#include <Preferences.h>
#include <Ticker.h>

// addons
#include <PubSubClient.h>

// Definierungen
#include "defs.h"
#include "lora_data.h"
#include "funktionen.h"
#include "lora_func.h"
/* ------------------------------------------*/












void setup() {
    Serial.begin(115200);
    Serial.println(F("Starting"));
    
    display.begin();
    display.setPowerSave(0);
    display.setFont(u8x8_font_chroma48medium8_r);
    sensor.begin();

    // Set up oversampling and filter initialization
    sensor.setTemperatureOversampling(BME680_OS_8X);
    sensor.setHumidityOversampling(BME680_OS_2X);
    sensor.setPressureOversampling(BME680_OS_4X);
    sensor.setIIRFilterSize(BME680_FILTER_SIZE_3);
    sensor.setGasHeater(320, 150); // 320*C for 150 ms

  
    preferences.begin("config", false);
    unsigned int counter = preferences.getUInt("counter", 0);
    // Increase counter by 1
    counter++;
    display.setCursor(0,5);
    display.print("Start-ups: ");
    display.print(counter);
    // Store the counter to the Preferences
    preferences.putUInt("counter", counter);
  
    // Close the Preferences
    preferences.end();
    
    
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

    
}

void loop() {
    os_runloop_once();
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  
    if ((millis() - lastTime) > timerDelay) {
      display.setContrast(255);
      getBME680Readings();
      display.setCursor(0,3); display.print("Temp.   = "); display.print(temperature);
      display.setCursor(0,4); display.print("Feuchte = "); display.print(humidity);
      display.setCursor(0,5); display.print("Druck   = "); display.print(pressure);
      display.setCursor(0,6); display.print("Gas     = "); display.print(gasResistance);
      Serial.printf("Temperature = %.2f ºC \n", temperature);
      Serial.printf("Humidity = %.2f % \n", humidity);
      Serial.printf("Pressure = %.2f hPa \n", pressure);
      Serial.printf("Gas Resistance = %.2f KOhm \n", gasResistance);
      Serial.println();
      Serial.printf("Humi aussen = %.2f % \n",ha);
      Serial.printf("temp aussen = %.2f % \n",ta);
      Serial.printf("press aussen = %.2f % \n",pa);
      Serial.printf("tau aussen = %.2f % \n",da);
      
//      // Send Events to the Web Server with the Sensor Readings
//      events.send("ping",NULL,millis());
//      events.send(String(temperature).c_str(),"temperature",millis());
//      events.send(String(humidity).c_str(),"humidity",millis());
//      events.send(String(pressure).c_str(),"pressure",millis());
//      events.send(String(gasResistance).c_str(),"gas",millis());
      
      lastTime = millis();
    }
    if ((millis() - dimTime ) > timerdim ){
      display.setContrast(0);
      dimTime = millis();
    }
}
