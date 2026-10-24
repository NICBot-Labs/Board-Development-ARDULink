// PROYEK 2 - BAGIAN ATMEGA328PB (PENGIRIM): SENSOR TEGANGAN -> KIRIM KE ESP32
// ALUR: setup() 1x -> loop() berulang:
// 1. TIAP 1 DETIK BACA PIN A0 SEBANYAK 20 KALI, LALU DIRATA-RATAKAN
// 2. V_ADC = ADC x VREF / 1023 ; TEGANGAN SUMBER = V_ADC x RASIO x KALIBRASI
// 3. KIRIM KE ESP32 LEWAT D8/D9 DENGAN FORMAT "V:12.34" + ENTER
// 4. TAMPILKAN JUGA DI SERIAL MONITOR (USB)

#include <SoftwareSerial.h>

const uint8_t PIN_SENSOR = A0;           // PIN SINYAL SENSOR TEGANGAN DI A0
const uint8_t PIN_RX_SW = 8;             // D8 = RX ATMEGA (MENERIMA DARI TX2 ESP32)
const uint8_t PIN_TX_SW = 9;             // D9 = TX ATMEGA (MENGIRIM KE RX2 ESP32)

const float VREF      = 5.0;             // TEGANGAN REFERENSI ADC (UKUR 5V BOARD DENGAN MULTIMETER, ISI HASILNYA)
const float RASIO     = 5.0;             // RASIO PEMBAGI TEGANGAN (CEK SKEMATIK BOARD; UBAH JIKA BERBEDA)
const float KALIBRASI = 1.0;             // FAKTOR KOREKSI (ISI SETELAH MEMBANDINGKAN DENGAN MULTIMETER)
const uint8_t JUMLAH_SAMPEL = 20;        // JUMLAH PEMBACAAN YANG DIRATA-RATAKAN

SoftwareSerial linkESP(PIN_RX_SW, PIN_TX_SW); // JALUR SERIAL KE ESP32

unsigned long waktuBaca = 0;             // WAKTU PEMBACAAN TERAKHIR (ms)

// FUNGSI: BACA ADC BEBERAPA KALI LALU KEMBALIKAN RATA-RATANYA
float bacaRataADC() {
  unsigned long total = 0;               // PENAMPUNG JUMLAH SEMUA PEMBACAAN
  for (uint8_t i = 0; i < JUMLAH_SAMPEL; i++) { // ULANGI SEBANYAK JUMLAH_SAMPEL
    total += analogRead(PIN_SENSOR);     // BACA ADC A0 (0-1023) LALU TAMBAHKAN
    delay(2);                            // DELAY 2 ms ANTAR PEMBACAAN (ADC LEBIH STABIL)
  }
  return (float)total / JUMLAH_SAMPEL;   // KEMBALIKAN RATA-RATA
}

void setup() {                           // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(9600);                    // SERIAL MONITOR (USB)
  linkESP.begin(9600);                   // JALUR KE ESP32 (BAUDRATE HARUS SAMA DI ESP32)
  Serial.println("Proyek 2: Sensor tegangan A0 -> ESP32 siap");
}

void loop() {                            // DIJALANKAN BERULANG TERUS
  if (millis() - waktuBaca >= 1000) {    // SUDAH 1 DETIK SEJAK PEMBACAAN TERAKHIR?
    waktuBaca = millis();                // CATAT WAKTU PEMBACAAN SEKARANG

    float adc  = bacaRataADC();                      // RATA-RATA NILAI ADC (0-1023)
    float vAdc = adc * VREF / 1023.0;                // TEGANGAN DI PIN A0
    float vIn  = vAdc * RASIO * KALIBRASI;           // TEGANGAN SUMBER YANG DIUKUR

    linkESP.print("V:");                 // PENANDA DATA TEGANGAN
    linkESP.println(vIn, 2);             // NILAI TEGANGAN 2 DESIMAL + ENTER

    Serial.print("ADC: ");               // TAMPILKAN DI SERIAL MONITOR
    Serial.print(adc, 1);
    Serial.print("  | V pin A0: ");
    Serial.print(vAdc, 3);
    Serial.print(" V  | V sumber: ");
    Serial.print(vIn, 2);
    Serial.println(" V  -> dikirim ke ESP32");
  }
}
