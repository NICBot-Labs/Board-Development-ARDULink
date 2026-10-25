#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
#define OLED_ADDRESS  0x3C

#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

const char *scrollText =
  "Board version V1.2, Edisi Universitas UNU Blitar..... Sistem Dual Mikrokontroler dengan total 3 Core, Sensor Suhu Internal LM35, Pewaktu Presisi Tinggi dengan RTC DS3231, Sensor tegangan, Indikator Visual Dual LED RGB, Layar Tampilan OLED 0.91 Inci, Komunikasi Antar-Mikrokontroler via UART, Kapasitas Ekspansi Luas dengan total 43 GPIO";

int16_t scrollX = SCREEN_WIDTH;
int16_t textWidth = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED SSD1306 tidak terdeteksi!");
    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
  textWidth = strlen(scrollText) * 6 + 20;
}

void loop() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(18, 3);
  display.println("ARDULink");
  display.setTextSize(1);
  display.setCursor(scrollX, 23);
  display.print(scrollText);
  display.display();
  scrollX--;
  if (scrollX < -(textWidth)) {
    scrollX = SCREEN_WIDTH;
  }
  delay(5); 
}