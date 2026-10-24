// ALUR: setup() 1x -> loop() berulang:
// 1. TUNGGU BYTE '1' DARI ATMEGA
// 2. TERIMA '1' -> LED GPIO4 NYALA 1 DETIK
// 3. LED MATI -> KIRIM BYTE '2' KE ATMEGA ("GILIRAN ATMEGA")
// 4. KEMBALI MENUNGGU

#define LED_PIN 4                          // LED BUILT-IN ESP32 DI GPIO4
#define RX2_PIN 16                         // GPIO16 = RX2 ESP32 (DARI TX ATMEGA, LEWAT PEMBAGI TEGANGAN)
#define TX2_PIN 17                         // GPIO17 = TX2 ESP32 (KE RX ATMEGA)

const unsigned long DURASI_NYALA = 1000;   // LAMA LED NYALA SETIAP GILIRAN (ms)

bool giliranSaya = false;                  // TRUE = SEKARANG GILIRAN ESP32
unsigned long waktuMulai = 0;              // WAKTU GILIRAN ESP32 DIMULAI (ms)

void setup() {                             // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  pinMode(LED_PIN, OUTPUT);                // ATUR GPIO4 SEBAGAI OUTPUT
  digitalWrite(LED_PIN, LOW);              // LED MATI SAAT AWAL
  Serial.begin(115200);                    // SERIAL MONITOR UNTUK DEBUG (115200)
  Serial2.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN); // UART2 KE ATMEGA, BAUD HARUS SAMA (9600)
  Serial.println("ESP32 siap, menunggu giliran"); // TANDA PROGRAM SUDAH JALAN
}

void loop() {                              // DIJALANKAN BERULANG TERUS
  if (!giliranSaya) {                      // ===== MENUNGGU GILIRAN =====
    while (Serial2.available()) {          // ADA BYTE MASUK DARI ATMEGA?
      char c = Serial2.read();             // BACA 1 BYTE
      if (c == '1') {                      // BYTE '1' = "GILIRAN ESP32"?
        digitalWrite(LED_PIN, HIGH);       // LED NYALA
        giliranSaya = true;                // TANDAI SEKARANG GILIRAN ESP32
        waktuMulai = millis();             // CATAT WAKTU GILIRAN DIMULAI
        Serial.println("Terima '1' -> LED ON"); // INFO DI SERIAL MONITOR
      }                                    // BYTE LAIN DIABAIKAN
    }
  } else {                                 // ===== GILIRAN ESP32 =====
    while (Serial2.available()) {          // BUANG BYTE YANG MASUK SELAMA GILIRAN ESP32
      Serial2.read();                      // BACA DAN ABAIKAN
    }
    if (millis() - waktuMulai >= DURASI_NYALA) { // LED SUDAH NYALA 1 DETIK?
      digitalWrite(LED_PIN, LOW);          // LED MATI
      Serial2.write('2');                  // KIRIM 1 BYTE '2' = "GILIRAN ATMEGA"
      giliranSaya = false;                 // GILIRAN ESP32 SELESAI
      Serial.println("LED OFF -> kirim '2'"); // INFO DI SERIAL MONITOR
    }
  }
}
