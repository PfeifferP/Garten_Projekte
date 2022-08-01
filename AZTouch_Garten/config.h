/* ------------------------------------------------------------
 * WiFi (NTP) und Datum/Zeit
 * ------------------------------------------------------------ */
// Default-Werte für einige Einstellungen
// werden genutzt, wenn kein passender Wert im NVS gefunden wid
#define DEFAULT_WIFI_TIMEOUT 60
#define DEFAULT_NTP_SERVER PSTR("de.pool.ntp.org")
#define DEFAULT_TIMEZONE PSTR("CET-1CEST,M3.5.0,M10.5.0/3")  // Europa/Berlin
#define DEFAULT_NTP_FREQ 24
int ntp_freq;

/* Timer für verschiedene Aufgaben:
 * - alle n Stunden die Uhrzeit per NTP-Abfrage aktualisieren
 * - ggf. weitere Timer, z.B. alle n Sekunden Sensoren abfragen,
 *   eine Anzeige aktualisieren etc.
 */
unsigned long timer_ntp=0,   //NTP-Server abfragen
              timer_time=0;  // Uhrzeit anzeigen

/* ------------------------------------------------------------ */
hw_timer_t * tim1 = NULL;
hw_timer_t * tim2 = NULL;

#define BKLED 15
#define BEEPER 26
#define CONFIG_FILE "/config.json"

struct settings {
  String ssid, wkey, ntps;
}
