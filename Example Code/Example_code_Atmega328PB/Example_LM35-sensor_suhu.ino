// ALUR: setup() 1x -> loop() berulang:
// 1. TIAP 1 DETIK BACA PIN A1 SEBANYAK 50 KALI, LALU DIRATA-RATAKAN (MENGURANGI NOISE)
// 2. UBAH NILAI ADC (0-1023) JADI TEGANGAN: mV = ADC x VREF x 1000 / 1023
// 3. LM35 = 10 mV PER DERAJAT -> SUHU (C) = mV / 10
// 4. TAMPILKAN DI SERIAL MONITOR

const uint8_t PIN_LM35 = A1;             // PIN OUTPUT (Vout) LM35 DI A1

const float VREF      = 5.0;             // TEGANGAN REFERENSI ADC (UKUR 5V BOARD DENGAN MULTIMETER, ISI HASILNYA)
const float MV_PER_C  = 10.0;            // SENSITIVITAS LM35: 10 mV PER 1 DERAJAT CELCIUS
const float OFFSET_C  = 0.0;             // KOREKSI SUHU DALAM C (ISI SETELAH MEMBANDINGKAN DENGAN TERMOMETER)
const uint8_t JUMLAH_SAMPEL = 50;        // JUMLAH PEMBACAAN YANG DIRATA-RATAKAN

unsigned long waktuBaca = 0;             // WAKTU PEMBACAAN TERAKHIR (ms)

// FUNGSI: BACA ADC BEBERAPA KALI LALU KEMBALIKAN RATA-RATANYA
float bacaRataADC() {
  unsigned long total = 0;               // PENAMPUNG JUMLAH SEMUA PEMBACAAN
  for (uint8_t i = 0; i < JUMLAH_SAMPEL; i++) { // ULANGI SEBANYAK JUMLAH_SAMPEL
    total += analogRead(PIN_LM35);       // BACA ADC A1 (0-1023) LALU TAMBAHKAN
    delay(2);                            // UNTUK DELAY 2 ms ANTAR PEMBACAAN (ADC LEBIH STABIL)
  }
  return (float)total / JUMLAH_SAMPEL;   // KEMBALIKAN RATA-RATA
}

void setup() {                           // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(9600);                    // SERIAL MONITOR UNTUK MENAMPILKAN HASIL (9600)
  Serial.println("LM35 di A1 siap");     // TANDA PROGRAM SUDAH JALAN
}

void loop() {                            // DIJALANKAN BERULANG TERUS
  if (millis() - waktuBaca >= 1000) {    // SUDAH 1 DETIK SEJAK PEMBACAAN TERAKHIR?
    waktuBaca = millis();                // CATAT WAKTU PEMBACAAN SEKARANG

    float adc  = bacaRataADC();                      // RATA-RATA NILAI ADC (0-1023)
    float mV   = adc * VREF * 1000.0 / 1023.0;       // TEGANGAN OUTPUT LM35 DALAM mV
    float suhu = mV / MV_PER_C + OFFSET_C;           // SUHU DALAM DERAJAT CELCIUS

    Serial.print("ADC: ");               // LABEL NILAI ADC
    Serial.print(adc, 1);                // NILAI ADC 1 DESIMAL
    Serial.print("  | Tegangan: ");      // LABEL TEGANGAN
    Serial.print(mV, 1);                 // TEGANGAN 1 DESIMAL
    Serial.print(" mV  | Suhu: ");       // LABEL SUHU
    Serial.print(suhu, 1);               // SUHU 1 DESIMAL
    Serial.println(" C");                // SATUAN + ENTER
  }
}
