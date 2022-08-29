void getBME680Readings(){
  // Tell BME680 to begin measurement.
  unsigned long endTime = BME680.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  if (!BME680.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }
  temperature = BME680.temperature;
  pressure = BME680.pressure / 100.0;
  humidity = BME680.humidity;
  gasResistance = BME680.gas_resistance / 1000.0;
}


String getSensorReadings(){
  DynamicJsonDocument json(1024);
  json["temp1"] = String(temperature);
  json["humi"] =  String(humidity);
  json["press"] = String(pressure);
  String jsonString;
  serializeJson(json, jsonString);
  return jsonString;
}

void GetGasReference() {
  // Now run the sensor for a burn-in period, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.
  //Serial.println("Getting a new gas reference value");
  int readings = 10;
  for (int i = 1; i <= readings; i++) { // read gas for 10 x 0.150mS = 1.5secs
    gas_reference += BME680.readGas();
  }
  gas_reference = gas_reference / readings;
  //Serial.println("Gas Reference = "+String(gas_reference,3));
}
