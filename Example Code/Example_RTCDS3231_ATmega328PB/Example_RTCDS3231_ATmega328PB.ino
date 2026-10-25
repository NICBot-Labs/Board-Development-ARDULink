#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);
  Serial.println("Membaca RTC DS3231...");
  if (!rtc.begin()) {
    Serial.println("RTC DS3231 tidak terdeteksi!");
    Serial.println("Periksa wiring SDA/SCL dan power.");
    while (1) {
      delay(1000);
    }
  }

  // Atur waktu RTC sesuai keinginan
  // Format: DateTime(Tahun, Bulan, Tanggal, Jam, Menit, Detik)
  // rtc.adjust(DateTime(2026, 10, 25, 14, 30, 0));

  if (rtc.lostPower()) {
    Serial.println("RTC kehilangan daya, set waktu ke waktu kompilasi...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  Serial.println("RTC siap.");
}

void loop() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");

  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);

  Serial.println();

  delay(1000);
}