// PROYEK 1 - BAGIAN ATMEGA328PB (PENGIRIM): LM35 -> KIRIM KE ESP32
// ALUR: setup() 1x -> loop() berulang:
// 1. TIAP 1 DETIK BACA PIN A1 SEBANYAK 50 KALI, LALU DIRATA-RATAKAN
// 2. HITUNG SUHU: mV = ADC x VREF x 1000 / 1023 ; SUHU (C) = mV / 10
// 3. KIRIM KE ESP32 LEWAT D8/D9 DENGAN FORMAT "T:28.5" + ENTER
// 4. TAMPILKAN JUGA DI SERIAL MONITOR (USB)

#include <SoftwareSerial.h>

const uint8_t PIN_LM35 = A1;             // PIN OUTPUT (Vout) LM35 DI A1
const uint8_t PIN_RX_SW = 8;             // D8 = RX ATMEGA (MENERIMA DARI TX2 ESP32)
const uint8_t PIN_TX_SW = 9;             // D9 = TX ATMEGA (MENGIRIM KE RX2 ESP32)

const float VREF      = 5.0;             // TEGANGAN REFERENSI ADC (UKUR 5V BOARD DENGAN MULTIMETER, ISI HASILNYA)
const float MV_PER_C  = 10.0;            // SENSITIVITAS LM35: 10 mV PER 1 DERAJAT CELCIUS
const float OFFSET_C  = 0.0;             // KOREKSI SUHU DALAM C (ISI SETELAH MEMBANDINGKAN DENGAN TERMOMETER)
const uint8_t JUMLAH_SAMPEL = 50;        // JUMLAH PEMBACAAN YANG DIRATA-RATAKAN

SoftwareSerial linkESP(PIN_RX_SW, PIN_TX_SW); // JALUR SERIAL KE ESP32

unsigned long waktuBaca = 0;             // WAKTU PEMBACAAN TERAKHIR (ms)

// FUNGSI: BACA ADC BEBERAPA KALI LALU KEMBALIKAN RATA-RATANYA
float bacaRataADC() {
  unsigned long total = 0;               // PENAMPUNG JUMLAH SEMUA PEMBACAAN
  for (uint8_t i = 0; i < JUMLAH_SAMPEL; i++) { // ULANGI SEBANYAK JUMLAH_SAMPEL
    total += analogRead(PIN_LM35);       // BACA ADC A1 (0-1023) LALU TAMBAHKAN
    delay(2);                            // DELAY 2 ms ANTAR PEMBACAAN (ADC LEBIH STABIL)
  }
  return (float)total / JUMLAH_SAMPEL;   // KEMBALIKAN RATA-RATA
}

void setup() {                           // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(9600);                    // SERIAL MONITOR (USB)
  linkESP.begin(9600);                   // JALUR KE ESP32 (BAUDRATE HARUS SAMA DI ESP32)
  Serial.println("Proyek 1: LM35 A1 -> ESP32 siap");
}

void loop() {                            // DIJALANKAN BERULANG TERUS
  if (millis() - waktuBaca >= 1000) {    // SUDAH 1 DETIK SEJAK PEMBACAAN TERAKHIR?
    waktuBaca = millis();                // CATAT WAKTU PEMBACAAN SEKARANG

    float adc  = bacaRataADC();                      // RATA-RATA NILAI ADC (0-1023)
    float mV   = adc * VREF * 1000.0 / 1023.0;       // TEGANGAN OUTPUT LM35 DALAM mV
    float suhu = mV / MV_PER_C + OFFSET_C;           // SUHU DALAM DERAJAT CELCIUS

    linkESP.print("T:");                 // PENANDA DATA SUHU
    linkESP.println(suhu, 1);            // NILAI SUHU 1 DESIMAL + ENTER

    Serial.print("ADC: ");               // TAMPILKAN DI SERIAL MONITOR
    Serial.print(adc, 1);
    Serial.print("  | Tegangan: ");
    Serial.print(mV, 1);
    Serial.print(" mV  | Suhu: ");
    Serial.print(suhu, 1);
    Serial.println(" C  -> dikirim ke ESP32");
  }
}
