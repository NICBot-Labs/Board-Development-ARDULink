// ALUR: setup() 1x -> loop() berulang:
// 1. TIAP 1 DETIK BACA PIN A0 SEBANYAK 20 KALI, LALU DIRATA-RATAKAN (MENGURANGI NOISE)
// 2. UBAH NILAI ADC (0-1023) JADI TEGANGAN PIN: V_ADC = ADC x VREF / 1023
// 3. KALIKAN RASIO PEMBAGI (5x) -> TEGANGAN SUMBER SEBENARNYA
// 4. TAMPILKAN DI SERIAL MONITOR

const uint8_t PIN_SENSOR = A0;           // PIN SINYAL (S) MODUL SENSOR TEGANGAN DI A0

const float VREF      = 5.0;             // TEGANGAN REFERENSI ADC (ATMEGA DIPOWER 5 V; UBAH JIKA BOARD ANDA 3,3 V)
const float RASIO     = 5.0;             // RASIO PEMBAGI MODUL: (30k + 7,5k) / 7,5k = 5
const float KALIBRASI = 1.0;             // FAKTOR KOREKSI (ISI SETELAH MEMBANDINGKAN DENGAN MULTIMETER)
const uint8_t JUMLAH_SAMPEL = 20;        // JUMLAH PEMBACAAN YANG DIRATA-RATAKAN

unsigned long waktuBaca = 0;             // WAKTU PEMBACAAN TERAKHIR (ms)

// FUNGSI: BACA ADC BEBERAPA KALI LALU KEMBALIKAN RATA-RATANYA
float bacaRataADC() {
  unsigned long total = 0;               // PENAMPUNG JUMLAH SEMUA PEMBACAAN
  for (uint8_t i = 0; i < JUMLAH_SAMPEL; i++) { // ULANGI SEBANYAK JUMLAH_SAMPEL
    total += analogRead(PIN_SENSOR);     // BACA ADC A0 (0-1023) LALU TAMBAHKAN
    delay(2);                            // UNTUK DELAY 2 ms ANTAR PEMBACAAN (ADC LEBIH STABIL)
  }
  return (float)total / JUMLAH_SAMPEL;   // KEMBALIKAN RATA-RATA
}

void setup() {                           // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(9600);                    // SERIAL MONITOR UNTUK MENAMPILKAN HASIL (9600)
  Serial.println("Sensor tegangan A0 siap"); // TANDA PROGRAM SUDAH JALAN
}

void loop() {                            // DIJALANKAN BERULANG TERUS
  if (millis() - waktuBaca >= 1000) {    // SUDAH 1 DETIK SEJAK PEMBACAAN TERAKHIR?
    waktuBaca = millis();                // CATAT WAKTU PEMBACAAN SEKARANG

    float adc  = bacaRataADC();                      // RATA-RATA NILAI ADC (0-1023)
    float vAdc = adc * VREF / 1023.0;                // TEGANGAN DI PIN A0 (0-5 V)
    float vIn  = vAdc * RASIO * KALIBRASI;           // TEGANGAN SUMBER YANG DIUKUR (0-25 V)

    Serial.print("ADC: ");               // LABEL NILAI ADC
    Serial.print(adc, 1);                // NILAI ADC 1 DESIMAL
    Serial.print("  | V pin A0: ");      // LABEL TEGANGAN DI PIN
    Serial.print(vAdc, 3);               // TEGANGAN PIN 3 DESIMAL
    Serial.print(" V  | V sumber: ");    // LABEL TEGANGAN SUMBER
    Serial.print(vIn, 2);                // TEGANGAN SUMBER 2 DESIMAL
    Serial.println(" V");                // SATUAN + ENTER
  }
}
