#include <Wire.h>

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>
#include <FSEditor.h>
#include <LittleFS.h>
#include <ESPConnect.h>
#include <ArduinoJson.h>
#include <Ticker.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>




// SKETCH BEGIN

const char* hostName = "esp-async";
const char* http_username = "admin";
const char* http_password = "admin";
const char* mqtt_server = "192.168.12.3";
const char* SensorID = "Sensor aussen";

Adafruit_BME280 BME280;

AsyncWebServer server(80);

AsyncEventSource events("/events");

Ticker webevent;
Ticker mqttevent;
 
float temperatur, feuchte, luftdruck, taupunkt;

WiFiClient espClient;
PubSubClient client(espClient);


#include "functions.h"
#include "event.h"
#include "webserver.h"


void setup(){
  Serial.begin(115200);
  delay(100);
  if (!BME280.begin(0x76)) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
  
  /* AutoConnect AP Configure SSID and password for Captive Portal */
  ESPConnect.autoConnect("ESPConfig");

  /* Begin connecting to previous WiFi or start autoConnect AP if unable to connect */
  if(ESPConnect.begin(&server)){
    Serial.println("Connected to WiFi");
    Serial.println("IPAddress: "+WiFi.localIP().toString());
  }else{
    Serial.println("Failed to connect to WiFi");
  }


  MDNS.addService("http","tcp",80);
 

  LittleFS.begin();

  
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!",NULL,millis(),1000);
  });
  server.addHandler(&events);

  server.addHandler(new FSEditor(http_username,http_password));

  
  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm");


  setupWebServer();
  setup_events();
  server.begin();
  client.setServer(mqtt_server, 1883);


}

void loop(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  
  
}
