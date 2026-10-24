// ALUR: setup() 1x -> loop() berulang:
// 1. CEK STATUS WIFI, JIKA TERPUTUS -> COBA SAMBUNG ULANG TIAP 15 DETIK
// 2. CEK PESAN DARI HP (BLUETOOTH) -> SIMPAN + BALAS KE HP
// 3. CEK DATA DARI ATMEGA (UART) -> JIKA "Hello World!" BALAS "Hello There!"
// 4. OLED 128x32 = 2 HALAMAN BERGANTI TIAP 3 DETIK:
//    HALAMAN 1 = WIFI + BLUETOOTH, HALAMAN 2 = PESAN ATMEGA + BALASAN + HP

#include <WiFi.h>                          // LIBRARY WIFI BAWAAN ESP32
#include <BluetoothSerial.h>               // LIBRARY BLUETOOTH CLASSIC (SPP) BAWAAN ESP32
#include <Wire.h>                          // LIBRARY I2C
#include <Adafruit_GFX.h>                  // LIBRARY GRAFIS DASAR (TEKS, GARIS, DLL)
#include <Adafruit_SSD1306.h>              // LIBRARY DRIVER OLED SSD1306

const char* WIFI_SSID = "NAMA_WIFI";       // GANTI DENGAN NAMA WIFI ANDA (HARUS 2.4 GHz)
const char* WIFI_PASS = "PASSWORD_WIFI";   // GANTI DENGAN PASSWORD WIFI ANDA
const char* NAMA_BT   = "ESP32-ATmega";    // NAMA PERANGKAT YANG MUNCUL DI BLUETOOTH HP

#define RX2_PIN 16                         // GPIO16 = RX2 ESP32 (DARI TX ATMEGA, LEWAT PEMBAGI TEGANGAN)
#define TX2_PIN 17                         // GPIO17 = TX2 ESP32 (KE RX ATMEGA)
#define SDA_PIN 22                         // SDA OLED DI GPIO22
#define SCL_PIN 21                         // SCL OLED DI GPIO21

#define LEBAR_OLED  128                    // LEBAR OLED DALAM PIXEL
#define TINGGI_OLED 32                     // TINGGI OLED 128x32 (4 BARIS TEKS UKURAN 1)
#define ALAMAT_OLED 0x3C                   // ALAMAT I2C OLED (UMUMNYA 0x3C, KADANG 0x3D)

// MEMBUAT OBJEK OLED: (LEBAR, TINGGI, OBJEK I2C, PIN RESET -1 = TIDAK DIPAKAI)
Adafruit_SSD1306 oled(LEBAR_OLED, TINGGI_OLED, &Wire, -1);
BluetoothSerial SerialBT;                  // OBJEK BLUETOOTH SERIAL

String pesanAtmega = "-";                  // PESAN TERAKHIR DARI ATMEGA
String pesanBalas  = "-";                  // BALASAN TERAKHIR DARI ESP32 KE ATMEGA
String pesanHP     = "-";                  // PESAN TERAKHIR DARI HP (BLUETOOTH)
uint32_t totalPesan = 0;                   // JUMLAH PESAN YANG DITERIMA DARI ATMEGA

unsigned long waktuOLED = 0;               // WAKTU UPDATE OLED TERAKHIR (ms)
unsigned long waktuCoba = 0;               // WAKTU PERCOBAAN KONEK WIFI TERAKHIR (ms)
unsigned long waktuPesanBaru = 0;          // WAKTU PESAN TERAKHIR MASUK (0 = BELUM ADA)
bool wifiSebelumnya = false;               // UNTUK DETEKSI PERUBAHAN STATUS WIFI
bool btSebelumnya   = false;               // UNTUK DETEKSI PERUBAHAN STATUS BLUETOOTH

// FUNGSI BANTU: POTONG TEKS AGAR TIDAK MELEBIHI LEBAR LAYAR (MAKS 13 KARAKTER SETELAH LABEL)
String potong(String teks) {
  if (teks.length() > 13) {                // TEKS LEBIH DARI 13 KARAKTER?
    return teks.substring(0, 13);          // YA -> AMBIL 13 KARAKTER PERTAMA
  }
  return teks;                             // TIDAK -> KEMBALIKAN APA ADANYA
}

// FUNGSI BANTU: MENAMPILKAN INFO DI OLED (2 HALAMAN)
void updateOLED() {
  oled.clearDisplay();                     // HAPUS ISI LAYAR (DI BUFFER)
  oled.setTextSize(1);                     // UKURAN TEKS KECIL (8 PIXEL PER BARIS, 21 KARAKTER)
  oled.setTextColor(SSD1306_WHITE);        // WARNA TEKS PUTIH (OLED MONOKROM)

  // TENTUKAN HALAMAN: 1 = PESAN (3 DETIK SETELAH PESAN BARU), SELAIN ITU BERGANTI TIAP 3 DETIK
  bool halamanPesan;
  if (waktuPesanBaru != 0 && millis() - waktuPesanBaru < 3000) {
    halamanPesan = true;                   // PESAN BARU -> LANGSUNG TAMPILKAN HALAMAN PESAN
  } else {
    halamanPesan = ((millis() / 3000) % 2 == 1); // BERGANTI TIAP 3 DETIK
  }

  if (!halamanPesan) {                     // ===== HALAMAN 1: WIFI + BLUETOOTH =====
    oled.setCursor(0, 0);                  // BARIS 1 (Y=0)
    if (WiFi.status() == WL_CONNECTED) {   // WIFI SUDAH TERHUBUNG?
      oled.print("WiFi: OK ");             // STATUS WIFI TERHUBUNG
      oled.print(WiFi.RSSI());             // KUAT SINYAL DALAM dBm
      oled.print("dBm");
      oled.setCursor(0, 8);                // BARIS 2 (Y=8)
      oled.print("IP:");                   // LABEL ALAMAT IP
      oled.print(WiFi.localIP());          // ALAMAT IP DARI ROUTER
    } else {                               // WIFI BELUM / TIDAK TERHUBUNG
      oled.print("WiFi: MENCARI...");      // STATUS WIFI SEDANG MENCARI
      oled.setCursor(0, 8);                // BARIS 2 (Y=8)
      oled.print(WIFI_SSID);               // NAMA WIFI YANG DITUJU
    }
    oled.setCursor(0, 16);                 // BARIS 3 (Y=16)
    oled.print("BT  : ");                  // LABEL BLUETOOTH
    oled.print(SerialBT.hasClient() ? "TERHUBUNG" : "MENUNGGU"); // STATUS BLUETOOTH
    oled.setCursor(0, 24);                 // BARIS 4 (Y=24)
    oled.print("Nama: ");                  // LABEL NAMA PERANGKAT
    oled.print(NAMA_BT);                   // NAMA BLUETOOTH ESP32
  } else {                                 // ===== HALAMAN 2: PESAN =====
    oled.setCursor(0, 0);                  // BARIS 1 (Y=0)
    oled.print("ATmega: ");                // LABEL PESAN DARI ATMEGA
    oled.print(potong(pesanAtmega));       // PESAN TERAKHIR DARI ATMEGA
    oled.setCursor(0, 8);                  // BARIS 2 (Y=8)
    oled.print("Balas : ");                // LABEL BALASAN KE ATMEGA
    oled.print(potong(pesanBalas));        // BALASAN TERAKHIR DARI ESP32
    oled.setCursor(0, 16);                 // BARIS 3 (Y=16)
    oled.print("HP    : ");                // LABEL PESAN DARI HP
    oled.print(potong(pesanHP));           // PESAN TERAKHIR DARI HP
    oled.setCursor(0, 24);                 // BARIS 4 (Y=24)
    oled.print("Total : ");                // LABEL JUMLAH PESAN
    oled.print(totalPesan);                // TOTAL PESAN DARI ATMEGA
  }

  oled.display();                          // KIRIM BUFFER KE OLED -> LAYAR BERUBAH
}

void setup() {                             // DIJALANKAN SATU KALI SAAT BOARD NYALA/RESET
  Serial.begin(115200);                    // SERIAL USB UNTUK DEBUG (SERIAL MONITOR 115200)
  Serial2.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN); // UART2 KE ATMEGA, BAUD HARUS SAMA (9600)

  Wire.begin(SDA_PIN, SCL_PIN);            // MULAI I2C DENGAN PIN KUSTOM (SDA=22, SCL=21)
  if (!oled.begin(SSD1306_SWITCHCAPVCC, ALAMAT_OLED)) { // INISIALISASI OLED, CEK BERHASIL?
    Serial.println("OLED tidak ditemukan! Cek wiring/alamat I2C"); // PESAN ERROR
    while (true) { delay(1000); }          // BERHENTI DI SINI JIKA OLED GAGAL
  }

  if (!SerialBT.begin(NAMA_BT)) {          // MULAI BLUETOOTH DENGAN NAMA PERANGKAT, CEK BERHASIL?
    Serial.println("Bluetooth gagal dimulai"); // PESAN ERROR
  } else {
    Serial.print("Bluetooth siap, nama: "); // INFO DI SERIAL MONITOR
    Serial.println(NAMA_BT);
  }

  WiFi.mode(WIFI_STA);                     // MODE STATION: ESP32 MENJADI CLIENT WIFI
  WiFi.setAutoReconnect(true);             // OTOMATIS SAMBUNG ULANG JIKA TERPUTUS
  WiFi.begin(WIFI_SSID, WIFI_PASS);        // MULAI KONEK KE WIFI (TIDAK MENUNGGU/BLOCKING)
  waktuCoba = millis();                    // CATAT WAKTU PERCOBAAN PERTAMA
  Serial.println("Menghubungkan WiFi...");  // INFO DI SERIAL MONITOR
}

void loop() {                              // DIJALANKAN BERULANG TERUS
  // --- BAGIAN WIFI: DETEKSI PERUBAHAN STATUS & COBA ULANG JIKA GAGAL ---
  bool wifiOk = (WiFi.status() == WL_CONNECTED); // CEK STATUS WIFI SEKARANG
  if (wifiOk && !wifiSebelumnya) {         // BARU SAJA TERHUBUNG?
    Serial.print("WiFi terhubung, IP: ");  // INFO DI SERIAL MONITOR
    Serial.println(WiFi.localIP());        // TAMPILKAN IP
  }
  if (!wifiOk && wifiSebelumnya) {         // BARU SAJA TERPUTUS?
    Serial.println("WiFi terputus");       // INFO DI SERIAL MONITOR
  }
  wifiSebelumnya = wifiOk;                 // SIMPAN STATUS UNTUK PENGECEKAN BERIKUTNYA

  if (!wifiOk && millis() - waktuCoba >= 15000) { // BELUM TERHUBUNG SETELAH 15 DETIK?
    WiFi.disconnect();                     // PUTUSKAN PERCOBAAN LAMA
    WiFi.begin(WIFI_SSID, WIFI_PASS);      // COBA KONEK ULANG
    waktuCoba = millis();                  // CATAT WAKTU PERCOBAAN BARU
    Serial.println("Coba konek ulang...");  // INFO DI SERIAL MONITOR
  }

  // --- BAGIAN BLUETOOTH: DETEKSI HP TERSAMBUNG / PUTUS ---
  bool btOk = SerialBT.hasClient();        // CEK ADA HP YANG TERSAMBUNG?
  if (btOk && !btSebelumnya) {             // BARU SAJA TERSAMBUNG?
    Serial.println("Bluetooth: HP terhubung"); // INFO DI SERIAL MONITOR
    SerialBT.println("ESP32 siap. Ketik pesan lalu kirim."); // SAPAAN KE HP
  }
  if (!btOk && btSebelumnya) {             // BARU SAJA PUTUS?
    Serial.println("Bluetooth: HP terputus"); // INFO DI SERIAL MONITOR
  }
  btSebelumnya = btOk;                     // SIMPAN STATUS UNTUK PENGECEKAN BERIKUTNYA

  // --- BAGIAN BLUETOOTH: TERIMA PESAN DARI HP ---
  if (SerialBT.available()) {              // ADA DATA MASUK DARI HP?
    String dataHP = SerialBT.readStringUntil('\n'); // BACA SAMPAI KARAKTER ENTER
    dataHP.trim();                         // HAPUS SPASI / '\r' / '\n' DI UJUNG TEKS
    if (dataHP.length() > 0) {             // ABAIKAN PESAN KOSONG
      pesanHP = dataHP;                    // SIMPAN UNTUK DITAMPILKAN DI OLED
      waktuPesanBaru = millis();           // CATAT WAKTU -> OLED PINDAH KE HALAMAN PESAN
      Serial.print("BT TERIMA : ");        // TAMPILKAN DI SERIAL MONITOR
      Serial.println(dataHP);
      SerialBT.print("Diterima: ");        // BALAS KE HP: AWALAN "Diterima: "
      SerialBT.println(dataHP);            // LANJUT DENGAN ISI PESAN + ENTER
      updateOLED();                        // LANGSUNG UPDATE OLED
    }
  }

  // --- BAGIAN UART: TERIMA DARI ATMEGA, BALAS, TERUSKAN KE HP ---
  if (Serial2.available()) {               // ADA DATA MASUK DARI ATMEGA?
    String data = Serial2.readStringUntil('\n'); // BACA SAMPAI KARAKTER ENTER
    data.trim();                           // HAPUS SPASI / '\r' / '\n' DI UJUNG TEKS
    Serial.print("TERIMA : ");             // TAMPILKAN DI SERIAL MONITOR
    Serial.println(data);

    pesanAtmega = data;                    // SIMPAN PESAN UNTUK DITAMPILKAN DI OLED
    totalPesan++;                          // NAIKKAN COUNTER PESAN
    waktuPesanBaru = millis();             // CATAT WAKTU -> OLED PINDAH KE HALAMAN PESAN

    if (data == "Hello World!") {          // CEK ISI PESAN SAMA PERSIS DENGAN "Hello World!"?
      Serial2.println("Hello There!");     // YA -> BALAS "Hello There!" + ENTER KE ATMEGA
      pesanBalas = "Hello There!";         // SIMPAN BALASAN UNTUK OLED
      Serial.println("BALAS  : Hello There!"); // TAMPILKAN BALASAN DI SERIAL MONITOR
    } else {                               // PESAN LAIN (BUKAN "Hello World!")
      pesanBalas = "-";                    // TIDAK ADA BALASAN
    }

    if (SerialBT.hasClient()) {            // ADA HP YANG TERSAMBUNG?
      SerialBT.print("ATmega -> ESP32: "); // KIRIM LOG KE HP: PESAN DARI ATMEGA
      SerialBT.println(data);
      if (pesanBalas != "-") {             // JIKA ADA BALASAN
        SerialBT.print("ESP32 -> ATmega: "); // KIRIM LOG KE HP: BALASAN KE ATMEGA
        SerialBT.println(pesanBalas);
      }
    }
    updateOLED();                          // LANGSUNG UPDATE OLED SAAT ADA PESAN BARU
  }

  // --- BAGIAN OLED: REFRESH BERKALA (GANTI HALAMAN & UPDATE RSSI) ---
  if (millis() - waktuOLED >= 500) {       // SUDAH 0,5 DETIK SEJAK UPDATE TERAKHIR?
    waktuOLED = millis();                  // CATAT WAKTU UPDATE SEKARANG
    updateOLED();                          // PERBARUI TAMPILAN OLED
  }
}
