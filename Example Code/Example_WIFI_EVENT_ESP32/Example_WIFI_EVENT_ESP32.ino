#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

#define LOGSYS true

#define WS2812_PIN 2    
#define WS2812_COUNT 1  

Adafruit_NeoPixel pixels(WS2812_COUNT, WS2812_PIN, NEO_GRB + NEO_KHZ800);

#if LOGSYS
#define logf(X) Serial.print(F(X))
#define logfln(X) Serial.println(F(X))
#define log(X) Serial.print(X)
#define logln(X) Serial.println(X)
#else
#define logf(X)
#define logfln(x)
#define log(X)
#define logln(x)
#endif

const char* SSID = "realme 12+";
const char* password = "abcdefgh";

int counterReconnect = 0;
int maxCounterReconnect = 10;

void setLedColor(uint8_t r, uint8_t g, uint8_t b) {
  pixels.setPixelColor(0, pixels.Color(r, g, b));
  pixels.show();
}

void statusWifiConnect() {
  setLedColor(0, 255, 0);   // hijau
}

void statusWifiDisconnect() {
  setLedColor(255, 0, 0);   // merah
}

void statusWifiRetry() {
  setLedColor(0, 0, 255);    // biru
}

void statusWifiError() {
  setLedColor(255, 255, 0);  // kuning
}

void EventWifi(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      statusWifiConnect();
      counterReconnect = 0;
      logfln("Terhubung Ke wifi");
      logf("IP Address : ");
      logln(WiFi.localIP());
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      statusWifiDisconnect();
      logfln("Tidak terhubung ke wifi");
      logfln("menghubungkan kembali");

      counterReconnect++;
      if (counterReconnect >= maxCounterReconnect) {
        logfln("Gagal Koneksi ke Wifi");
        statusWifiError();
        delay(1000);
        ESP.restart();
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);
  pixels.begin();
  pixels.setBrightness(50);  
  setLedColor(255, 255, 255); 
  delay(300);
  statusWifiDisconnect();

  logfln("Start Sistem");
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.onEvent(EventWifi);
  WiFi.begin(SSID, password);
}

void loop() {
  Serial.println("Sistem Run");
  if (WiFi.status() != WL_CONNECTED) {
    statusWifiRetry();
  }
  delay(500);
}