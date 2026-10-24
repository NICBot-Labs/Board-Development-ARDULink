// ALUR: setup() 1x -> loop() berulang:
// JIKA ADA PESAN DARI ATMEGA -> TAMPILKAN -> BALAS "ACK:<pesan>"

#define RX2_PIN 16                        // GPIO16 = RX2 ESP32 (DARI TX ATMEGA, LEWAT PEMBAGI TEGANGAN)
#define TX2_PIN 17                        // GPIO17 = TX2 ESP32 (KE RX ATMEGA)

void setup() {                            // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(115200);                   // SERIAL USB UNTUK DEBUG (SERIAL MONITOR 115200)
  Serial2.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN); // UART2: BAUD 9600, 8 DATA BIT, NO PARITY, 1 STOP BIT, PIN DIATUR MANUAL
  Serial.println("ESP32 siap");           // TANDA PROGRAM SUDAH JALAN
}

void loop() {                             // DIJALANKAN BERULANG TERUS
  if (Serial2.available()) {              // ADA DATA MASUK DARI ATMEGA?
    String data = Serial2.readStringUntil('\n'); // BACA SAMPAI KARAKTER ENTER
    data.trim();                          // HAPUS SPASI / '\r' / '\n' DI UJUNG TEKS
    Serial.print("TERIMA : ");            // TAMPILKAN PESAN DI SERIAL MONITOR
    Serial.println(data);

    Serial2.print("ACK:");                // BALAS KE ATMEGA: AWALAN "ACK:"
    Serial2.println(data);                // LANJUT DENGAN ISI PESAN YANG DITERIMA + ENTER
    Serial.print("BALAS  : ACK:");        // TAMPILKAN BALASAN DI SERIAL MONITOR
    Serial.println(data);
  }
}
