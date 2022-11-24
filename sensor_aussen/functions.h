String getSensorReadings(){
  
  DynamicJsonDocument json(1024);
  json["temp1"] = String(BME280.readTemperature());
  json["humi"] =  String(BME280.readHumidity());
  json["press"] = String(BME280.readPressure()/100);
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
      client.publish("sensoren/aussen", "online");
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
