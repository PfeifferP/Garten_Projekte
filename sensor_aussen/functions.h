String getSensorReadings(){
  
  DynamicJsonDocument json(1024);
  
  temperatur = BME280.readTemperature();
  feuchte = BME280.readHumidity();
  luftdruck = BME280.readPressure() / 100.0F;
  taupunkt = 243.04 * (log(feuchte/100.0) + ((17.625 * temperatur)/(243.04 + temperatur)))/(17.625 - log(feuchte/100.0) - ((17.625 * temperatur)/(243.04 + temperatur)));
   
  json["temperatur"] = String(temperatur);
  json["feuchte"] =  String(feuchte);
  json["luftdruck"] = String(luftdruck);
  json["taupunkt"] = String(taupunkt);
  String jsonString;
  serializeJson(json, jsonString);
  return jsonString;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Sensor_aussen-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("sensoren/aussen/status", "online");
      client.publish("sensoren/aussen/ip", WiFi.localIP().toString().c_str());
      // ... and resubscribe
      client.subscribe("sensoren/aussen/command");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
