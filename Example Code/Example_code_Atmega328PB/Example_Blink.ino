// ALUR: setup() 1x -> loop() berulang: LED ON -> tunggu -> LED OFF -> tunggu -> ulang

const uint8_t LED_PIN = 13;   // KONSTANTA NOMOR PIN LED (D13 / PB5)

void setup() {                // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  pinMode(LED_PIN, OUTPUT);   // ATUR PIN 13 SEBAGAI OUTPUT
}

void loop() {                 // DIJALANKAN BERULANG TERUS
  digitalWrite(LED_PIN, HIGH); // PIN 5 V -> LED NYALA
  delay(1000);                // UNTUK DELAY 1 DETIK (LED TETAP NYALA)
  digitalWrite(LED_PIN, LOW);  // PIN 0 V -> LED MATI
  delay(1000);                // UNTUK DELAY 1 DETIK (LED TETAP MATI)
}                             // KEMBALI KE AWAL loop() -> LED BERKEDIP
