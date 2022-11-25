

void cron2(){
  // Sende Sensor Daten alle 10 Sek.
    events.send("ping",NULL,millis());
    events.send(getSensorReadings().c_str(),"new_readings" ,millis());
  
}

void cron3(){
  // Sende Sensor Daten alle 30 Sek. zum MQTT-Server
  client.publish("sensoren/aussen/status", "online");
  client.publish("sensoren/aussen/temperatur", String(temperatur).c_str());
  client.publish("sensoren/aussen/feuchte", String(feuchte).c_str());
  client.publish("sensoren/aussen/luftdruck", String(luftdruck).c_str());
  client.publish("sensoren/aussen/taupunkt", String(taupunkt).c_str());
}

void setup_events(){
  webevent.attach(10, cron2);
  mqttevent.attach(30, cron3);
}
  
