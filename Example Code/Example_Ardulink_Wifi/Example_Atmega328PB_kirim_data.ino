// ALUR: setup() 1x -> loop() berulang:
// 1. TIAP 2 DETIK KIRIM "Hello World!" KE ESP32
// 2. JIKA ADA BALASAN DARI ESP32 ("Hello There!") -> TAMPILKAN DI SERIAL MONITOR

#include <SoftwareSerial.h>              // LIBRARY SERIAL VIA SOFTWARE (PIN BEBAS)

const uint8_t PIN_RX = 9;                // D9 = RX ATMEGA (DARI TX ESP32 GPIO17)
const uint8_t PIN_TX = 8;                // D8 = TX ATMEGA (KE RX ESP32 GPIO16, LEWAT PEMBAGI TEGANGAN)

SoftwareSerial espSerial(PIN_RX, PIN_TX); // OBJEK SERIAL KE ESP32 (URUTAN: RX, TX)

unsigned long waktuKirim = 0;            // MENYIMPAN WAKTU PENGIRIMAN TERAKHIR (ms)

void setup() {                           // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(9600);                    // SERIAL USB/UART0 UNTUK DEBUG (SERIAL MONITOR)
  espSerial.begin(9600);                 // SERIAL KE ESP32, BAUD HARUS SAMA DENGAN ESP32 (9600)
  Serial.println("ATmega328PB siap");    // TANDA PROGRAM SUDAH JALAN
}

void loop() {                            // DIJALANKAN BERULANG TERUS
  // --- BAGIAN KIRIM: SETIAP 2 DETIK, TANPA delay() AGAR PENERIMAAN TIDAK TERHALANG ---
  if (millis() - waktuKirim >= 2000) {   // CEK SUDAH 2 DETIK SEJAK PENGIRIMAN TERAKHIR?
    waktuKirim = millis();               // CATAT WAKTU PENGIRIMAN SEKARANG
    espSerial.println("Hello World!");   // KIRIM "Hello World!" + ENTER ('\n') SEBAGAI PENANDA AKHIR PESAN
    Serial.println("KIRIM  : Hello World!"); // TAMPILKAN DI SERIAL MONITOR
  }

  // --- BAGIAN TERIMA: BACA BALASAN DARI ESP32 ---
  if (espSerial.available()) {           // ADA DATA MASUK DARI ESP32?
    String data = espSerial.readStringUntil('\n'); // BACA SAMPAI KARAKTER ENTER
    data.trim();                         // HAPUS SPASI / '\r' / '\n' DI UJUNG TEKS
    Serial.print("TERIMA : ");           // TAMPILKAN BALASAN DI SERIAL MONITOR
    Serial.println(data);                // SEHARUSNYA "Hello There!"
  }
}
