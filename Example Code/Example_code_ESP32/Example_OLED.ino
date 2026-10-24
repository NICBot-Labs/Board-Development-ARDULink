// ALUR: setup() 1x -> loop() berulang:
// 1. setup(): MULAI I2C -> INISIALISASI OLED -> TAMPILKAN LAYAR AWAL
// 2. loop(): TIAP 1 DETIK -> HITUNG UPTIME + COUNTER -> TAMPILKAN DI OLED

#include <Wire.h>                          // LIBRARY I2C
#include <Adafruit_GFX.h>                  // LIBRARY GRAFIS DASAR (TEKS, GARIS, DLL)
#include <Adafruit_SSD1306.h>              // LIBRARY DRIVER OLED SSD1306

#define SDA_PIN 22                         // SDA OLED DI GPIO22
#define SCL_PIN 21                         // SCL OLED DI GPIO21

#define LEBAR_OLED  128                    // LEBAR OLED DALAM PIXEL
#define TINGGI_OLED 32                     // TINGGI OLED 128x32 (4 BARIS TEKS UKURAN 1)
#define ALAMAT_OLED 0x3C                   // ALAMAT I2C OLED (UMUMNYA 0x3C, KADANG 0x3D)

// MEMBUAT OBJEK OLED: (LEBAR, TINGGI, OBJEK I2C, PIN RESET -1 = TIDAK DIPAKAI)
Adafruit_SSD1306 oled(LEBAR_OLED, TINGGI_OLED, &Wire, -1);

unsigned long waktuUpdate = 0;             // WAKTU UPDATE OLED TERAKHIR (ms)
uint32_t counter = 0;                      // COUNTER YANG BERTAMBAH TIAP DETIK

void setup() {                             // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(115200);                    // SERIAL MONITOR UNTUK DEBUG (115200)

  Wire.begin(SDA_PIN, SCL_PIN);            // MULAI I2C DENGAN PIN KUSTOM (SDA=22, SCL=21)

  if (!oled.begin(SSD1306_SWITCHCAPVCC, ALAMAT_OLED)) { // INISIALISASI OLED, CEK BERHASIL?
    Serial.println("OLED tidak ditemukan! Cek wiring/alamat I2C"); // PESAN ERROR
    while (true) { delay(1000); }          // BERHENTI DI SINI JIKA OLED GAGAL
  }

  oled.clearDisplay();                     // BERSIHKAN LAYAR
  oled.setTextSize(1);                     // UKURAN TEKS KECIL (8 PIXEL PER BARIS, 21 KARAKTER)
  oled.setTextColor(SSD1306_WHITE);        // WARNA TEKS PUTIH (OLED MONOKROM)
  oled.setCursor(0, 0);                    // POSISI TEKS: X=0, Y=0 (POJOK KIRI ATAS)
  oled.println("ESP32 + OLED siap");       // TAMPILAN AWAL
  oled.display();                          // KIRIM BUFFER KE OLED -> TEKS MUNCUL
  Serial.println("OLED siap");             // TANDA PROGRAM SUDAH JALAN
  delay(1000);                             // UNTUK DELAY 1 DETIK (TAMPILAN AWAL TERLIHAT)
}

void loop() {                              // DIJALANKAN BERULANG TERUS
  if (millis() - waktuUpdate >= 1000) {    // SUDAH 1 DETIK SEJAK UPDATE TERAKHIR?
    waktuUpdate = millis();                // CATAT WAKTU UPDATE SEKARANG
    counter++;                             // NAIKKAN COUNTER

    oled.clearDisplay();                   // HAPUS ISI LAYAR (DI BUFFER)
    oled.setCursor(0, 0);                  // BARIS 1 (Y=0)
    oled.println("ESP32 + OLED 128x32");   // JUDUL
    oled.setCursor(0, 8);                  // BARIS 2 (Y=8)
    oled.println("Hello World!");          // TEKS CONTOH
    oled.setCursor(0, 16);                 // BARIS 3 (Y=16)
    oled.print("Uptime : ");               // LABEL WAKTU NYALA
    oled.print(millis() / 1000);           // WAKTU SEJAK BOARD NYALA DALAM DETIK
    oled.println(" dtk");
    oled.setCursor(0, 24);                 // BARIS 4 (Y=24)
    oled.print("Counter: ");               // LABEL COUNTER
    oled.print(counter);                   // NILAI COUNTER
    oled.display();                        // KIRIM BUFFER KE OLED -> LAYAR BERUBAH

    Serial.print("Counter: ");             // INFO DI SERIAL MONITOR
    Serial.println(counter);
  }
}
