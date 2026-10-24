// ALUR: setup() 1x -> loop() berulang:
// 1. setup(): MULAI I2C -> CARI RTC -> JIKA RTC KEHILANGAN DAYA, ATUR WAKTU DARI WAKTU KOMPILASI
// 2. loop(): TIAP 1 DETIK BACA WAKTU DARI RTC -> TAMPILKAN DI SERIAL MONITOR

#include <Wire.h>                        // LIBRARY I2C (A4 = SDA, A5 = SCL)
#include <RTClib.h>                      // LIBRARY RTC (ADAFRUIT)

RTC_DS3231 rtc;                          // OBJEK RTC DS3231 (UNTUK DS1307 GANTI KE: RTC_DS1307 rtc;)

const char* NAMA_HARI[] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"}; // NAMA HARI (0 = MINGGU)

unsigned long waktuBaca = 0;             // WAKTU PEMBACAAN TERAKHIR (ms)

void setup() {                           // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(9600);                    // SERIAL MONITOR UNTUK MENAMPILKAN WAKTU (9600)
  Wire.begin();                          // MULAI I2C DI PIN DEFAULT (A4 = SDA, A5 = SCL)

  if (!rtc.begin()) {                    // RTC TIDAK TERDETEKSI DI JALUR I2C?
    Serial.println("RTC tidak ditemukan! Cek wiring SDA(A4)/SCL(A5)"); // PESAN ERROR
    while (true) { delay(1000); }        // BERHENTI DI SINI JIKA RTC GAGAL
  }

  if (rtc.lostPower()) {                 // RTC KEHILANGAN DAYA (BATERAI HABIS / BARU DIPASANG)? (DS1307: if (!rtc.isrunning()))
    Serial.println("RTC kehilangan daya, waktu diatur ulang"); // INFO DI SERIAL MONITOR
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // ATUR WAKTU RTC = WAKTU SAAT KODE DIKOMPILASI
  }

  Serial.println("RTC siap");            // TANDA PROGRAM SUDAH JALAN
}

void loop() {                            // DIJALANKAN BERULANG TERUS
  if (millis() - waktuBaca >= 1000) {    // SUDAH 1 DETIK SEJAK PEMBACAAN TERAKHIR?
    waktuBaca = millis();                // CATAT WAKTU PEMBACAAN SEKARANG

    DateTime now = rtc.now();            // BACA WAKTU SEKARANG DARI RTC

    char buf[40];                        // WADAH TEKS UNTUK HASIL FORMAT
    snprintf(buf, sizeof(buf), "%s, %02d/%02d/%04d %02d:%02d:%02d",
             NAMA_HARI[now.dayOfTheWeek()], // NAMA HARI
             now.day(), now.month(), now.year(),         // TANGGAL/BULAN/TAHUN
             now.hour(), now.minute(), now.second());    // JAM:MENIT:DETIK
    Serial.print(buf);                   // TAMPILKAN WAKTU DI SERIAL MONITOR

    Serial.print("  | Suhu RTC: ");      // LABEL SUHU (KHUSUS DS3231, HAPUS 3 BARIS INI UNTUK DS1307)
    Serial.print(rtc.getTemperature());  // SUHU DARI SENSOR DI DALAM DS3231
    Serial.println(" C");
  }
}
