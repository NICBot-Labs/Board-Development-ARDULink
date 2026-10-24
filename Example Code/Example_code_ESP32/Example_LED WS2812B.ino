// ALUR: setup() 1x -> loop() berulang:
// SEMUA LED MERAH -> HIJAU -> BIRU -> ULANG, MASING-MASING 1 DETIK

#include <Adafruit_NeoPixel.h>    // MEMANGGIL LIBRARY UNTUK KONTROL WS2812B

const uint8_t PIN_DATA    = 2;    // PIN DATA WS2812B TERHUBUNG KE GPIO2 (D2)
const uint16_t JUMLAH_LED = 8;    // JUMLAH LED PADA STRIP (SESUAIKAN DENGAN HARDWARE)

// MEMBUAT OBJEK STRIP: (JUMLAH LED, PIN DATA, TIPE LED)
// NEO_GRB    = URUTAN WARNA WS2812B ADALAH GREEN-RED-BLUE
// NEO_KHZ800 = KECEPATAN DATA 800 kHz
Adafruit_NeoPixel strip(JUMLAH_LED, PIN_DATA, NEO_GRB + NEO_KHZ800);

// FUNGSI BANTU: MENGATUR SEMUA LED MENJADI SATU WARNA
void warnaSemua(uint32_t warna) {
  for (uint16_t i = 0; i < JUMLAH_LED; i++) { // ULANGI DARI LED KE-0 SAMPAI TERAKHIR
    strip.setPixelColor(i, warna);            // ATUR WARNA LED KE-i (BELUM TAMPIL)
  }
  strip.show();                               // KIRIM DATA KE STRIP -> LED BERUBAH WARNA
}

void setup() {                    // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(115200);           // SERIAL MONITOR UNTUK DEBUG (115200)
  strip.begin();                  // INISIALISASI PIN DATA DAN STRIP
  strip.setBrightness(50);        // KECERAHAN 0-255 (DIBATASI AGAR ARUS AMAN)
  strip.show();                   // KIRIM DATA AWAL -> SEMUA LED MATI
  Serial.println("WS2812B GPIO2 siap"); // TANDA PROGRAM SUDAH JALAN
}

void loop() {                     // DIJALANKAN BERULANG TERUS
  warnaSemua(strip.Color(255, 0, 0)); // SEMUA LED MERAH (R, G, B)
  Serial.println("MERAH");        // INFO DI SERIAL MONITOR
  delay(1000);                    // UNTUK DELAY 1 DETIK
  warnaSemua(strip.Color(0, 255, 0)); // SEMUA LED HIJAU
  Serial.println("HIJAU");        // INFO DI SERIAL MONITOR
  delay(1000);                    // UNTUK DELAY 1 DETIK
  warnaSemua(strip.Color(0, 0, 255)); // SEMUA LED BIRU
  Serial.println("BIRU");         // INFO DI SERIAL MONITOR
  delay(1000);                    // UNTUK DELAY 1 DETIK
}                                 // KEMBALI KE AWAL loop() -> WARNA BERGANTI TERUS
