// PROYEK 1 - BAGIAN ESP32 (PENERIMA): TERIMA SUHU DARI ATMEGA -> TAMPILKAN DI OLED
// ALUR: setup() 1x -> loop() berulang:
// 1. BACA SERIAL2 (GPIO16 RX2 / GPIO17 TX2) SAMPAI ENTER
// 2. JIKA FORMAT "T:xx.x" -> AMBIL NILAI SUHU
// 3. TAMPILKAN DI OLED (I2C: SDA GPIO21, SCL GPIO22)
// 4. JIKA > 3 DETIK TIDAK ADA DATA -> OLED TAMPILKAN PERINGATAN
//
// LIBRARY (ARDUINO IDE -> LIBRARY MANAGER): "Adafruit SSD1306" DAN "Adafruit GFX Library"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const uint8_t LEBAR_OLED  = 128;         // LEBAR OLED (PIKSEL)
const uint8_t TINGGI_OLED = 64;          // TINGGI OLED (64 ATAU 32, SESUAI MODUL ANDA)
const uint8_t ALAMAT_OLED = 0x3C;        // ALAMAT I2C OLED (UMUMNYA 0x3C)

const int PIN_RX2 = 16;                  // GPIO16 = RX2 (DARI D9 ATMEGA)
const int PIN_TX2 = 17;                  // GPIO17 = TX2 (KE D8 ATMEGA)
const int PIN_SDA = 21;                  // GPIO21 = SDA
const int PIN_SCL = 22;                  // GPIO22 = SCL

const unsigned long BATAS_TUNGGU = 3000; // BATAS WAKTU TANPA DATA (ms)

Adafruit_SSD1306 oled(LEBAR_OLED, TINGGI_OLED, &Wire, -1); // OBJEK OLED (TANPA PIN RESET)

float suhu = 0.0;                        // SUHU TERAKHIR YANG DITERIMA
unsigned long waktuTerima = 0;           // WAKTU DATA TERAKHIR DITERIMA (ms)
bool adaData = false;                    // SUDAH PERNAH MENERIMA DATA?

// FUNGSI: TAMPILKAN SUHU DI OLED
void tampilSuhu() {
  oled.clearDisplay();                   // BERSIHKAN LAYAR
  oled.setTextColor(SSD1306_WHITE);      // WARNA TEKS PUTIH
  oled.setTextSize(1);                   // UKURAN TEKS KECIL
  oled.setCursor(0, 0);
  oled.println("SUHU (LM35)");           // JUDUL
  oled.setTextSize(3);                   // UKURAN TEKS BESAR UNTUK ANGKA
  oled.setCursor(0, 24);
  oled.print(suhu, 1);                   // NILAI SUHU 1 DESIMAL
  oled.setTextSize(2);
  oled.print(" C");                      // SATUAN
  oled.display();                        // KIRIM KE LAYAR
}

// FUNGSI: TAMPILKAN PESAN BILA TIDAK ADA DATA
void tampilTanpaData() {
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.println("SUHU (LM35)");
  oled.setCursor(0, 28);
  oled.println("Menunggu data dari");
  oled.println("ATmega...");
  oled.display();
}

void setup() {                           // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(115200);                  // SERIAL MONITOR ESP32 (USB)
  Serial2.begin(9600, SERIAL_8N1, PIN_RX2, PIN_TX2); // JALUR KE ATMEGA (9600, SAMA DENGAN ATMEGA)
  Wire.begin(PIN_SDA, PIN_SCL);          // MULAI I2C

  if (!oled.begin(SSD1306_SWITCHCAPVCC, ALAMAT_OLED)) { // MULAI OLED
    Serial.println("OLED tidak ditemukan! Cek kabel/alamat I2C");
    while (true) delay(1000);            // BERHENTI JIKA OLED TIDAK ADA
  }
  tampilTanpaData();                     // TAMPILAN AWAL
  Serial.println("Proyek 1: ESP32 + OLED siap");
}

void loop() {                            // DIJALANKAN BERULANG TERUS
  if (Serial2.available()) {             // ADA DATA MASUK DARI ATMEGA?
    String baris = Serial2.readStringUntil('\n'); // BACA SATU BARIS
    baris.trim();                        // BUANG SPASI/ENTER SISA

    if (baris.startsWith("T:")) {        // FORMAT BENAR?
      suhu = baris.substring(2).toFloat(); // AMBIL ANGKA SETELAH "T:"
      waktuTerima = millis();            // CATAT WAKTU TERIMA
      adaData = true;
      tampilSuhu();                      // UPDATE OLED
      Serial.print("Suhu diterima: ");
      Serial.print(suhu, 1);
      Serial.println(" C");
    }
  }

  if (adaData && millis() - waktuTerima > BATAS_TUNGGU) { // DATA SUDAH LAMA TIDAK DATANG?
    adaData = false;
    tampilTanpaData();                   // TAMPILKAN PERINGATAN
  }
}
