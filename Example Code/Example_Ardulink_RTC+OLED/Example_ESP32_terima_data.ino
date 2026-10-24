// ALUR: setup() 1x -> loop() berulang:
// 1. setup(): MULAI I2C -> CARI RTC -> JIKA RTC KEHILANGAN DAYA, ATUR WAKTU DARI WAKTU KOMPILASI
// 2. loop(): TIAP 1 DETIK BACA RTC -> KIRIM 1 BARIS "RTC,thn,bln,tgl,hari,jam,mnt,dtk,suhu" KE ESP32
//            -> TAMPILKAN JUGA DI SERIAL MONITOR

#include <Wire.h>                        // LIBRARY I2C (A4 = SDA, A5 = SCL)
#include <RTClib.h>                      // LIBRARY RTC (ADAFRUIT)
#include <SoftwareSerial.h>              // LIBRARY SERIAL VIA SOFTWARE (PIN BEBAS)

const uint8_t PIN_RX = 9;                // D9 = RX ATMEGA (TIDAK DIPAKAI UNTUK DATA RTC)
const uint8_t PIN_TX = 8;                // D8 = TX ATMEGA (KE RX ESP32 GPIO16, LEWAT PEMBAGI TEGANGAN)

SoftwareSerial espSerial(PIN_RX, PIN_TX); // OBJEK SERIAL KE ESP32 (URUTAN: RX, TX)
RTC_DS3231 rtc;                          // OBJEK RTC DS3231 (UNTUK DS1307 GANTI KE: RTC_DS1307 rtc;)

unsigned long waktuKirim = 0;            // WAKTU PENGIRIMAN TERAKHIR (ms)

void setup() {                           // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(9600);                    // SERIAL MONITOR UNTUK DEBUG (9600)
  espSerial.begin(9600);                 // SERIAL KE ESP32, BAUD HARUS SAMA DENGAN ESP32 (9600)
  Wire.begin();                          // MULAI I2C DI PIN DEFAULT (A4 = SDA, A5 = SCL)

  if (!rtc.begin()) {                    // RTC TIDAK TERDETEKSI DI JALUR I2C?
    Serial.println("RTC tidak ditemukan! Cek wiring SDA(A4)/SCL(A5)"); // PESAN ERROR
    while (true) { delay(1000); }        // BERHENTI DI SINI JIKA RTC GAGAL
  }

  if (rtc.lostPower()) {                 // RTC KEHILANGAN DAYA? (DS1307: if (!rtc.isrunning()))
    Serial.println("RTC kehilangan daya, waktu diatur ulang"); // INFO DI SERIAL MONITOR
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // ATUR WAKTU RTC = WAKTU SAAT KODE DIKOMPILASI
  }

  Serial.println("ATmega + RTC siap");   // TANDA PROGRAM SUDAH JALAN
}

void loop() {                            // DIJALANKAN BERULANG TERUS
  if (millis() - waktuKirim >= 1000) {   // SUDAH 1 DETIK SEJAK PENGIRIMAN TERAKHIR?
    waktuKirim = millis();               // CATAT WAKTU PENGIRIMAN SEKARANG

    DateTime now = rtc.now();            // BACA WAKTU SEKARANG DARI RTC
    float suhu = rtc.getTemperature();   // BACA SUHU DARI DS3231 (HAPUS UNTUK DS1307, KIRIM 0)

    // --- KIRIM KE ESP32: SATU BARIS, FIELD DIPISAH KOMA ---
    espSerial.print("RTC,");             // PENANDA AWAL DATA
    espSerial.print(now.year());   espSerial.print(',');  // TAHUN
    espSerial.print(now.month());  espSerial.print(',');  // BULAN
    espSerial.print(now.day());    espSerial.print(',');  // TANGGAL
    espSerial.print(now.dayOfTheWeek()); espSerial.print(','); // HARI (0 = MINGGU)
    espSerial.print(now.hour());   espSerial.print(',');  // JAM
    espSerial.print(now.minute()); espSerial.print(',');  // MENIT
    espSerial.print(now.second()); espSerial.print(',');  // DETIK
    espSerial.println(suhu, 1);          // SUHU 1 DESIMAL + ENTER ('\n') SEBAGAI PENANDA AKHIR BARIS

    // --- TAMPILKAN DI SERIAL MONITOR UNTUK DEBUG ---
    Serial.print("KIRIM : ");            // LABEL
    Serial.print(now.day());    Serial.print('/');        // TANGGAL
    Serial.print(now.month());  Serial.print('/');        // BULAN
    Serial.print(now.year());   Serial.print(' ');        // TAHUN
    Serial.print(now.hour());   Serial.print(':');        // JAM
    Serial.print(now.minute()); Serial.print(':');        // MENIT
    Serial.println(now.second());                         // DETIK
  }
}
