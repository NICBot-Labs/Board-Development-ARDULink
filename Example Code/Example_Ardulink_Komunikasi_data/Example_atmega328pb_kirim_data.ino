// ALUR: setup() 1x -> loop() berulang:
// 1. TIAP 1 DETIK KIRIM "PING:n" KE ESP32
// 2. JIKA ADA DATA MASUK DARI ESP32 -> BACA DAN TAMPILKAN DI SERIAL MONITOR

#include <SoftwareSerial.h>              // LIBRARY SERIAL VIA SOFTWARE (PIN BEBAS)

const uint8_t PIN_RX = 8;                // D9 = RX ATMEGA (DARI TX ESP32)
const uint8_t PIN_TX = 9;                // D8 = TX ATMEGA (KE RX ESP32)

SoftwareSerial espSerial(PIN_RX, PIN_TX); // OBJEK SERIAL KE ESP32 (URUTAN: RX, TX)

unsigned long waktuKirim = 0;            // MENYIMPAN WAKTU PENGIRIMAN TERAKHIR (ms)
uint16_t hitung = 0;                     // COUNTER PESAN YANG DIKIRIM

void setup() {                           // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(9600);                    // SERIAL USB/UART0 UNTUK DEBUG (SERIAL MONITOR)
  espSerial.begin(9600);                 // SERIAL KE ESP32, BAUD HARUS SAMA DI KEDUA SISI
  Serial.println("ATmega328PB siap");    // TANDA PROGRAM SUDAH JALAN
}

void loop() {                            // DIJALANKAN BERULANG TERUS
  // --- BAGIAN KIRIM: SETIAP 1 DETIK, TANPA delay() AGAR PENERIMAAN TIDAK TERHALANG ---
  if (millis() - waktuKirim >= 1000) {   // CEK SUDAH 1 DETIK SEJAK PENGIRIMAN TERAKHIR?
    waktuKirim = millis();               // CATAT WAKTU PENGIRIMAN SEKARANG
    hitung++;                            // NAIKKAN COUNTER
    espSerial.print("PING:");            // KIRIM TEKS "PING:" KE ESP32
    espSerial.println(hitung);           // KIRIM ANGKA + ENTER ('\n') SEBAGAI PENANDA AKHIR PESAN
    Serial.print("KIRIM  : PING:");      // TAMPILKAN DI SERIAL MONITOR
    Serial.println(hitung);
  }

  // --- BAGIAN TERIMA: BACA BALASAN DARI ESP32 ---
  if (espSerial.available()) {           // ADA DATA MASUK DARI ESP32?
    String data = espSerial.readStringUntil('\n'); // BACA SAMPAI KARAKTER ENTER
    data.trim();                         // HAPUS SPASI / '\r' / '\n' DI UJUNG TEKS
    Serial.print("TERIMA : ");           // TAMPILKAN BALASAN DI SERIAL MONITOR
    Serial.println(data);
  }
}
