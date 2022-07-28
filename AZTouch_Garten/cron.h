//////////////////////// Timer Settings ////////////////////////////////////////////////
int cron_1 = 1;  //5 Seconds intervall for timer 1;
int cron_2 = 5; //20 Seconds intervall for timer 2;
int cron_3 = 30; //30 Seconds intervall for timer 3;
int cron_4 = 60;
int cron_5 = 300;
//////////////////////// End Timer Settings ////////////////////////////////////////////
long cront1, cront2, cront3, cront4, cront5;






void cron1() {
  //Your Cronjob action here
  
}

void cron2() {
  //Your Cronjob action here
  drawWifiQuality();
}

void cron3() {
  //Your Cronjob action here
  
}

void cron4() {
  //Your Cronjob action here
  
}

void cron5() {
  //Your Cronjob action here
  
}

void cronjob() {
  long tmp = millis();
  if ((cront1 + (cron_1 * 1000)) <= tmp) { cron1(); cront1 = millis(); }
  if ((cront2 + (cron_2 * 1000)) <= tmp) { cron2(); cront2 = millis(); }
  if ((cront3 + (cron_3 * 1000)) <= tmp) { cron3(); cront3 = millis(); }
  if ((cront4 + (cron_4 * 1000)) <= tmp) { cron4(); cront4 = millis(); }
  if ((cront5 + (cron_5 * 1000)) <= tmp) { cron5(); cront5 = millis(); }
}
