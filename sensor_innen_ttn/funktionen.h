void callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic,"sensoren/aussen/feuchte") == 0){
    payload[length] = '\0';
    String s = String((char*)payload);
    ha = s.toFloat();
  }
  if(strcmp(topic,"sensoren/aussen/temperatur") == 0){
    payload[length] = '\0';
    String s = String((char*)payload);
    ta = s.toFloat();
  }
  if(strcmp(topic,"sensoren/aussen/luftdruck") == 0){
    payload[length] = '\0';
    String s = String((char*)payload);
    pa = s.toFloat();
  }
  if(strcmp(topic,"sensoren/aussen/taupunkt") == 0){
    payload[length] = '\0';
    String s = String((char*)payload);
    da = s.toFloat();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Sensor_innen-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("sensoren/innen/status", "online");
      // ... and resubscribe
      client.subscribe("sensoren/aussen/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}

void getBME680Readings(){
  // Tell BME680 to begin measurement.
  unsigned long endTime = sensor.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  if (!sensor.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }
  ti = sensor.temperature;
  pi = sensor.pressure / 100.0;
  hi = sensor.humidity;
  di = 243.04 * (log(hi/100.0) + ((17.625 * ti)/(243.04 + ti)))/(17.625 - log(hi/100.0) - ((17.625 * ti)/(243.04 + ti)));
  gi = sensor.gas_resistance / 1000.0;

  display.setCursor(0,3); display.print(" "); display.print(ti); display.print("  "); display.print(ta);
  display.setCursor(0,4); display.print(" "); display.print(hi); display.print("  "); display.print(ha);
  display.setCursor(0,5); display.print(" "); display.print(pi); display.print("  "); display.print(pa);
  display.setCursor(0,6); display.print(" "); display.print(di); display.print("  "); display.print(da);
  display.setCursor(0,7); display.print(" "); display.print(gi);

  // Sende Sensor Daten alle 60 Sek. zum MQTT-Server
  client.publish("sensoren/innen/status", "online");
  client.publish("sensoren/innen/temperatur", String(ti).c_str());
  client.publish("sensoren/innen/feuchte", String(hi).c_str());
  client.publish("sensoren/innen/luftdruck", String(pi).c_str());
  client.publish("sensoren/innen/taupunkt", String(di).c_str());
  client.publish("sensoren/innen/gas", String(gi).c_str());

}
