// ALUR: setup() 1x -> loop() berulang:
// 1. CEK DATA MASUK DARI ATMEGA (UART2)
// 2. JIKA PESAN = "Hello World!" -> BALAS "Hello There!"
// 3. TAMPILKAN PESAN MASUK DAN BALASAN DI OLED + SERIAL MONITOR

#include <Wire.h>                          // LIBRARY I2C
#include <Adafruit_GFX.h>                  // LIBRARY GRAFIS DASAR (TEKS, GARIS, DLL)
#include <Adafruit_SSD1306.h>              // LIBRARY DRIVER OLED SSD1306

#define RX2_PIN 16                         // GPIO16 = RX2 ESP32 (DARI TX ATMEGA, LEWAT PEMBAGI TEGANGAN)
#define TX2_PIN 17                         // GPIO17 = TX2 ESP32 (KE RX ATMEGA)
#define SDA_PIN 22                         // SDA OLED DI GPIO22
#define SCL_PIN 21                         // SCL OLED DI GPIO21

#define LEBAR_OLED  128                    // LEBAR OLED DALAM PIXEL
#define TINGGI_OLED 64                     // TINGGI OLED DALAM PIXEL (UBAH KE 32 JIKA OLED 128x32)
#define ALAMAT_OLED 0x3C                   // ALAMAT I2C OLED (UMUMNYA 0x3C, KADANG 0x3D)

// MEMBUAT OBJEK OLED: (LEBAR, TINGGI, OBJEK I2C, PIN RESET -1 = TIDAK DIPAKAI)
Adafruit_SSD1306 oled(LEBAR_OLED, TINGGI_OLED, &Wire, -1);

// FUNGSI BANTU: MENAMPILKAN PESAN MASUK DAN BALASAN DI OLED
void tampilkanOLED(String masuk, String balas) {
  oled.clearDisplay();                     // HAPUS ISI LAYAR (DI BUFFER)
  oled.setTextSize(1);                     // UKURAN TEKS KECIL (1x)
  oled.setTextColor(SSD1306_WHITE);        // WARNA TEKS PUTIH (OLED MONOKROM)

  oled.setCursor(0, 0);                    // POSISI TEKS: POJOK KIRI ATAS
  oled.println("Dari ATmega:");            // LABEL PESAN MASUK
  oled.setCursor(0, 12);                   // POSISI ISI PESAN MASUK
  oled.println(masuk);                     // TAMPILKAN "Hello World!"

  oled.drawLine(0, 30, 127, 30, SSD1306_WHITE); // GARIS PEMISAH

  oled.setCursor(0, 36);                   // POSISI LABEL BALASAN
  oled.println("Balasan ESP32:");          // LABEL PESAN BALASAN
  oled.setCursor(0, 48);                   // POSISI ISI BALASAN
  oled.println(balas);                     // TAMPILKAN "Hello There!"

  oled.display();                          // KIRIM BUFFER KE OLED -> LAYAR BERUBAH
}

void setup() {                             // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(115200);                    // SERIAL USB UNTUK DEBUG (SERIAL MONITOR 115200)
  Serial2.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN); // UART2 KE ATMEGA, BAUD HARUS SAMA (9600)

  Wire.begin(SDA_PIN, SCL_PIN);            // MULAI I2C DENGAN PIN KUSTOM (SDA=22, SCL=21)

  if (!oled.begin(SSD1306_SWITCHCAPVCC, ALAMAT_OLED)) { // INISIALISASI OLED, CEK BERHASIL?
    Serial.println("OLED tidak ditemukan! Cek wiring/alamat I2C"); // PESAN ERROR
    while (true) { delay(1000); }          // BERHENTI DI SINI JIKA OLED GAGAL
  }

  oled.clearDisplay();                     // BERSIHKAN LAYAR
  oled.setTextSize(1);                     // UKURAN TEKS KECIL
  oled.setTextColor(SSD1306_WHITE);        // WARNA TEKS PUTIH
  oled.setCursor(0, 0);                    // POSISI TEKS AWAL
  oled.println("ESP32 siap");              // TAMPILAN AWAL
  oled.println("Menunggu data...");        // PETUNJUK MENUNGGU DATA DARI ATMEGA
  oled.display();                          // KIRIM KE OLED
  Serial.println("ESP32 siap");            // TANDA PROGRAM SUDAH JALAN
}

void loop() {                              // DIJALANKAN BERULANG TERUS
  if (Serial2.available()) {               // ADA DATA MASUK DARI ATMEGA?
    String data = Serial2.readStringUntil('\n'); // BACA SAMPAI KARAKTER ENTER
    data.trim();                           // HAPUS SPASI / '\r' / '\n' DI UJUNG TEKS

    Serial.print("TERIMA : ");             // TAMPILKAN DI SERIAL MONITOR
    Serial.println(data);

    if (data == "Hello World!") {          // CEK ISI PESAN SAMA PERSIS DENGAN "Hello World!"?
      Serial2.println("Hello There!");     // YA -> BALAS "Hello There!" + ENTER KE ATMEGA
      Serial.println("BALAS  : Hello There!"); // TAMPILKAN BALASAN DI SERIAL MONITOR
      tampilkanOLED(data, "Hello There!"); // TAMPILKAN PESAN MASUK + BALASAN DI OLED
    } else {                               // PESAN LAIN (BUKAN "Hello World!")
      Serial.println("Pesan tidak dikenal"); // ABAIKAN DAN BERI TAHU DI SERIAL MONITOR
    }
  }
}
