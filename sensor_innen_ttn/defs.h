// OLED Pins
#define OLED_SCL 15   // GPIO 15
#define OLED_SDA  4   // GPIO  4
#define OLED_RST 16   // GPIO 16

// BME680 Pins
#define BME_SDA 23
#define BME_SCL 22

//LoRa Pins
#define LoRa_RST  14  // GPIO 14
#define LoRa_CS   18  // GPIO 18
#define LoRa_DIO0 26  // GPIO 26
#define LoRa_DIO1 35  // GPIO 35
#define LoRa_DIO2 34  // GPIO 34

// WiFi
const char* ssid = "devnet-34";
const char* password = "testerwlan";
const char* mqtt_server = "192.168.12.3";
int counts = 10;
int lorapacket = 0;

Preferences preferences;

// define the display type 
U8X8_SSD1306_128X64_NONAME_SW_I2C display(/* clock=*/ OLED_SCL, /* data=*/ OLED_SDA, /* reset=*/ OLED_RST);

Adafruit_BME680 sensor;

WiFiClient espClient;
PubSubClient client(espClient);


// Pins LoRa
const lmic_pinmap lmic_pins = {.nss = LoRa_CS, .rxtx = LMIC_UNUSED_PIN, .rst = LoRa_RST, .dio = { LoRa_DIO0, LoRa_DIO1, LoRa_DIO2 }, };

float ti, hi, pi, di, gi, ta, ha, pa, da;

unsigned long lastTime = 0;  
unsigned long timerDelay = 30000;  // send readings timer
unsigned long dimTime = 0;
unsigned long timerdim = 15000;
