// ALUR: setup() 1x -> loop() berulang:
// LED NYALA 1 DETIK -> LED MATI 1 DETIK -> ULANG

const uint8_t LED_PIN = 4;     // LED BUILT-IN TERHUBUNG KE GPIO4 (D4)

void setup() {                 // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  pinMode(LED_PIN, OUTPUT);    // ATUR GPIO4 SEBAGAI OUTPUT
  Serial.begin(115200);        // SERIAL MONITOR UNTUK DEBUG (115200)
  Serial.println("LED built-in GPIO4 siap"); // TANDA PROGRAM SUDAH JALAN
}

void loop() {                  // DIJALANKAN BERULANG TERUS
  digitalWrite(LED_PIN, HIGH); // PIN 3,3 V -> LED NYALA
  Serial.println("LED ON");    // INFO DI SERIAL MONITOR
  delay(1000);                 // UNTUK DELAY 1 DETIK (LED TETAP NYALA)
  digitalWrite(LED_PIN, LOW);  // PIN 0 V -> LED MATI
  Serial.println("LED OFF");   // INFO DI SERIAL MONITOR
  delay(1000);                 // UNTUK DELAY 1 DETIK (LED TETAP MATI)
}                              // KEMBALI KE AWAL loop() -> LED BERKEDIP
