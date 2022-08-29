/*   ---   Timersettings   ---   */
int cron_1 = 1;
int cron_2 = 5;
int cron_3 = 30;
int cron_4 = 60;
int cron_5 = 300;

long cront1, cront2, cront3, cront4, cront5;

/* ------------------------------ */

void cron1(){
  
  
}
void cron2(){
  // Sende Sensor Daten alle 10 Sek.
//    events.send("ping",NULL,millis());
//    events.send(getSensorReadings().c_str(),"new_readings" ,millis());
  getBME680Readings();
}
void cron3(){
  Serial.println("Sensor Readings:");
  Serial.println("  Temperature = " + String(temperature, 1)     + "°C");
  Serial.println("     Pressure = " + String(pressure ) + " hPa");
  Serial.println("     Humidity = " + String(humidity, 1)        + "%");
  Serial.println("          Gas = " + String(gasResistance )               + " ohms\n");
  Serial.println();
  
}
void cron4(){
  
}
void cron5(){
  
}
/* ------------------------------ */
void cronjob(){
  long tmp = millis();
  if ((cront1 + (cron_1 * 1000)) <= tmp) { cron1(); cront1 = millis();}
  if ((cront2 + (cron_2 * 1000)) <= tmp) { cron2(); cront2 = millis();}
  if ((cront3 + (cron_3 * 1000)) <= tmp) { cron3(); cront3 = millis();}
  if ((cront4 + (cron_4 * 1000)) <= tmp) { cron4(); cront4 = millis();}
  if ((cront5 + (cron_5 * 1000)) <= tmp) { cron5(); cront5 = millis();}
}  
