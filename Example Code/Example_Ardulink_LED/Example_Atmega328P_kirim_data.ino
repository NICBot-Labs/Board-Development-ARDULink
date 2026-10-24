// ALUR: setup() 1x -> loop() berulang (STATE MACHINE):
// STATE NYALA    : LED D13 NYALA 1 DETIK -> LED MATI -> KIRIM BYTE '1' KE ESP32 -> PINDAH KE MENUNGGU
// STATE MENUNGGU : TUNGGU BYTE '2' DARI ESP32 -> LED NYALA LAGI -> KEMBALI KE NYALA
// JIKA 5 DETIK TIDAK ADA '2' -> MULAI ULANG GILIRAN SENDIRI (PENGAMAN JIKA ESP32 RESET)

#include <SoftwareSerial.h>              // LIBRARY SERIAL VIA SOFTWARE (PIN BEBAS)

const uint8_t PIN_RX  = 9;               // D9 = RX ATMEGA (DARI TX ESP32 GPIO17)
const uint8_t PIN_TX  = 8;               // D8 = TX ATMEGA (KE RX ESP32 GPIO16, LEWAT PEMBAGI TEGANGAN)
const uint8_t PIN_LED = 13;              // D13 = LED ATMEGA

const unsigned long DURASI_NYALA = 1000; // LAMA LED NYALA SETIAP GILIRAN (ms)
const unsigned long BATAS_TUNGGU = 5000; // BATAS WAKTU MENUNGGU BALASAN ESP32 (ms)

SoftwareSerial espSerial(PIN_RX, PIN_TX); // OBJEK SERIAL KE ESP32 (URUTAN: RX, TX)

enum Status { NYALA, MENUNGGU };         // DUA KEADAAN PROGRAM
Status status = NYALA;                   // KEADAAN SAAT INI, MULAI DARI NYALA
unsigned long waktuMulai = 0;            // WAKTU KEADAAN SAAT INI DIMULAI (ms)

void setup() {                           // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  pinMode(PIN_LED, OUTPUT);              // ATUR PIN LED SEBAGAI OUTPUT
  espSerial.begin(9600);                 // SERIAL KE ESP32, BAUD HARUS SAMA DENGAN ESP32 (9600)
  delay(2000);                           // UNTUK DELAY 2 DETIK, MENUNGGU ESP32 SELESAI BOOT
  digitalWrite(PIN_LED, HIGH);           // LED NYALA = GILIRAN PERTAMA DIMULAI DI ATMEGA
  waktuMulai = millis();                 // CATAT WAKTU GILIRAN DIMULAI
}

void loop() {                            // DIJALANKAN BERULANG TERUS
  if (status == NYALA) {                 // ===== GILIRAN ATMEGA =====
    if (millis() - waktuMulai >= DURASI_NYALA) { // LED SUDAH NYALA 1 DETIK?
      digitalWrite(PIN_LED, LOW);        // LED MATI
      espSerial.write('1');              // KIRIM 1 BYTE '1' = "GILIRAN ESP32"
      status = MENUNGGU;                 // PINDAH KE KEADAAN MENUNGGU
      waktuMulai = millis();             // CATAT WAKTU MULAI MENUNGGU
    }
  } else {                               // ===== MENUNGGU GILIRAN KEMBALI =====
    while (espSerial.available()) {      // ADA BYTE MASUK DARI ESP32?
      char c = espSerial.read();         // BACA 1 BYTE
      if (c == '2') {                    // BYTE '2' = "GILIRAN ATMEGA"?
        digitalWrite(PIN_LED, HIGH);     // LED NYALA
        status = NYALA;                  // PINDAH KE KEADAAN NYALA
        waktuMulai = millis();           // CATAT WAKTU MULAI NYALA
      }                                  // BYTE LAIN DIABAIKAN
    }
    if (status == MENUNGGU && millis() - waktuMulai >= BATAS_TUNGGU) { // 5 DETIK TANPA BALASAN?
      digitalWrite(PIN_LED, HIGH);       // MULAI ULANG GILIRAN: LED NYALA
      status = NYALA;                    // PINDAH KE KEADAAN NYALA
      waktuMulai = millis();             // CATAT WAKTU MULAI NYALA
    }
  }
}
