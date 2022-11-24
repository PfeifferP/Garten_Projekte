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

const char * hostName = "esp-async";
const char* http_username = "admin";
const char* http_password = "admin";
const char* mqtt_server = "192.168.12.3";
const char* SensorID = "Sensor aussen";

Adafruit_BME280 BME280;

AsyncWebServer server(80);

AsyncEventSource events("/events");

Ticker webevent;
Ticker mqttevent;
 


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

  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
  server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char*)data);
    if(final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index)
      Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char*)data);
    if(index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });


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
