

void cron2(){
  // Sende Sensor Daten alle 10 Sek.
    events.send("ping",NULL,millis());
    events.send(getSensorReadings().c_str(),"new_readings" ,millis());
  
}

void cron3(){
  client.publish("sensoren/aussen", "online");
  client.publish("sensoren/aussen/temperatur", String(BME280.readTemperature()).c_str());
}

void setup_events(){
  webevent.attach(10, cron2);
  mqttevent.attach(30, cron3);
}
  
