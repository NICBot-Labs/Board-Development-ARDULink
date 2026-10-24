// ALUR: setup() 1x -> loop() berulang:
// 1. CEK STATUS WIFI, JIKA TERPUTUS -> COBA SAMBUNG ULANG TIAP 15 DETIK
// 2. JIKA WIFI SUDAH TERHUBUNG -> JALANKAN WEB SERVER (BUKA IP DI BROWSER)
// 3. WEB: "/" = HALAMAN, "/data" = DATA TERBARU, "/kirim?pesan=..." = KIRIM KE ATMEGA
// 4. CEK DATA DARI ATMEGA (UART): JIKA "Hello World!" -> BALAS "Hello There!"
// 5. OLED 128x32 = 2 HALAMAN BERGANTI TIAP 3 DETIK

#include <WiFi.h>                          // LIBRARY WIFI BAWAAN ESP32
#include <WebServer.h>                     // LIBRARY WEB SERVER BAWAAN ESP32
#include <Wire.h>                          // LIBRARY I2C
#include <Adafruit_GFX.h>                  // LIBRARY GRAFIS DASAR (TEKS, GARIS, DLL)
#include <Adafruit_SSD1306.h>              // LIBRARY DRIVER OLED SSD1306

const char* WIFI_SSID = "NAMA_WIFI";       // GANTI DENGAN NAMA WIFI ANDA (HARUS 2.4 GHz)
const char* WIFI_PASS = "PASSWORD_WIFI";   // GANTI DENGAN PASSWORD WIFI ANDA

#define RX2_PIN 16                         // GPIO16 = RX2 ESP32 (DARI TX ATMEGA, LEWAT PEMBAGI TEGANGAN)
#define TX2_PIN 17                         // GPIO17 = TX2 ESP32 (KE RX ATMEGA)
#define SDA_PIN 22                         // SDA OLED DI GPIO22
#define SCL_PIN 21                         // SCL OLED DI GPIO21

#define LEBAR_OLED  128                    // LEBAR OLED DALAM PIXEL
#define TINGGI_OLED 32                     // TINGGI OLED 128x32 (4 BARIS TEKS UKURAN 1)
#define ALAMAT_OLED 0x3C                   // ALAMAT I2C OLED (UMUMNYA 0x3C, KADANG 0x3D)

// MEMBUAT OBJEK OLED: (LEBAR, TINGGI, OBJEK I2C, PIN RESET -1 = TIDAK DIPAKAI)
Adafruit_SSD1306 oled(LEBAR_OLED, TINGGI_OLED, &Wire, -1);
WebServer server(80);                      // OBJEK WEB SERVER DI PORT 80 (HTTP)

// HALAMAN WEB (DISIMPAN SEBAGAI TEKS; JAVASCRIPT MENGAMBIL /data TIAP 2 DETIK)
const char HALAMAN[] = R"rawliteral(
<!DOCTYPE html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 - ATmega</title>
<style>
body{font-family:sans-serif;max-width:420px;margin:20px auto;padding:0 12px}
.box{border:1px solid #ccc;border-radius:8px;padding:10px;margin:10px 0}
input{width:65%;padding:6px} button{padding:6px 10px}
</style></head><body>
<h2>ESP32 &harr; ATmega328PB</h2>
<div class="box">
<b>Dari ATmega:</b> <span id="a">-</span><br>
<b>Balasan ESP32:</b> <span id="b">-</span><br>
<b>Total pesan:</b> <span id="t">0</span><br>
<b>WiFi:</b> <span id="w">-</span>
</div>
<div class="box">
<input id="p" placeholder="Pesan ke ATmega" maxlength="50">
<button onclick="kirim()">Kirim</button><br><br>
<button onclick="cmd('LED:ON')">LED ON</button>
<button onclick="cmd('LED:OFF')">LED OFF</button>
</div>
<script>
function ambil(){
  fetch('/data').then(r=>r.text()).then(t=>{
    const x=t.split('\n');
    document.getElementById('a').textContent=x[0];
    document.getElementById('b').textContent=x[1];
    document.getElementById('t').textContent=x[2];
    document.getElementById('w').textContent=x[3]+' dBm';
  });
}
function cmd(c){ fetch('/kirim?pesan='+encodeURIComponent(c)); }
function kirim(){
  const e=document.getElementById('p');
  if(e.value.trim()!==''){ cmd(e.value); e.value=''; }
}
ambil(); setInterval(ambil,2000);
</script></body></html>
)rawliteral";

String pesanAtmega = "-";                  // PESAN TERAKHIR DARI ATMEGA
String pesanBalas  = "-";                  // BALASAN TERAKHIR DARI ESP32 KE ATMEGA
String pesanWeb    = "-";                  // PESAN TERAKHIR DARI BROWSER (WEB)
uint32_t totalPesan = 0;                   // JUMLAH PESAN YANG DITERIMA DARI ATMEGA

unsigned long waktuOLED = 0;               // WAKTU UPDATE OLED TERAKHIR (ms)
unsigned long waktuCoba = 0;               // WAKTU PERCOBAAN KONEK WIFI TERAKHIR (ms)
unsigned long waktuPesanBaru = 0;          // WAKTU PESAN TERAKHIR MASUK (0 = BELUM ADA)
bool wifiSebelumnya = false;               // UNTUK DETEKSI PERUBAHAN STATUS WIFI
bool serverJalan    = false;               // PENANDA WEB SERVER SUDAH DIMULAI

// FUNGSI BANTU: POTONG TEKS AGAR TIDAK MELEBIHI LEBAR LAYAR (MAKS 13 KARAKTER)
String potong(String teks) {
  if (teks.length() > 13) {                // TEKS LEBIH DARI 13 KARAKTER?
    return teks.substring(0, 13);          // YA -> AMBIL 13 KARAKTER PERTAMA
  }
  return teks;                             // TIDAK -> KEMBALIKAN APA ADANYA
}

// HANDLER WEB: HALAMAN UTAMA "/"
void handleRoot() {
  server.send(200, "text/html", HALAMAN);  // KIRIM HALAMAN HTML KE BROWSER
}

// HANDLER WEB: DATA TERBARU "/data" (TEKS, 1 BARIS PER FIELD)
void handleData() {
  String out = pesanAtmega + "\n" + pesanBalas + "\n" + String(totalPesan) + "\n" + String(WiFi.RSSI());
  server.send(200, "text/plain", out);     // KIRIM DATA SEBAGAI TEKS BIASA
}

// HANDLER WEB: KIRIM PESAN KE ATMEGA "/kirim?pesan=..."
void handleKirim() {
  String pesan = server.arg("pesan");      // AMBIL ISI PARAMETER "pesan"
  pesan.replace("\r", "");                 // BUANG KARAKTER ENTER AGAR TIDAK MERUSAK PROTOKOL
  pesan.replace("\n", "");                 // BUANG KARAKTER BARIS BARU
  pesan.trim();                            // HAPUS SPASI DI UJUNG
  if (pesan.length() == 0 || pesan.length() > 50) { // KOSONG ATAU TERLALU PANJANG?
    server.send(400, "text/plain", "pesan tidak valid"); // TOLAK
    return;                                // KELUAR DARI FUNGSI
  }
  Serial2.println(pesan);                  // KIRIM KE ATMEGA + ENTER
  pesanWeb = pesan;                        // SIMPAN UNTUK OLED
  waktuPesanBaru = millis();               // CATAT WAKTU -> OLED PINDAH KE HALAMAN PESAN
  Serial.print("WEB KIRIM : ");            // TAMPILKAN DI SERIAL MONITOR
  Serial.println(pesan);
  server.send(200, "text/plain", "terkirim"); // BALAS KE BROWSER
}

// FUNGSI BANTU: MENAMPILKAN INFO DI OLED (2 HALAMAN)
void updateOLED() {
  oled.clearDisplay();                     // HAPUS ISI LAYAR (DI BUFFER)
  oled.setTextSize(1);                     // UKURAN TEKS KECIL (8 PIXEL PER BARIS, 21 KARAKTER)
  oled.setTextColor(SSD1306_WHITE);        // WARNA TEKS PUTIH (OLED MONOKROM)

  int halaman;                             // NOMOR HALAMAN: 0 ATAU 1
  if (waktuPesanBaru != 0 && millis() - waktuPesanBaru < 3000) {
    halaman = 1;                           // PESAN BARU -> TAMPILKAN HALAMAN PESAN 3 DETIK
  } else {
    halaman = (millis() / 3000) % 2;       // BERGANTI TIAP 3 DETIK
  }

  if (halaman == 0) {                      // ===== HALAMAN 1: STATUS WIFI =====
    oled.setCursor(0, 0);                  // BARIS 1 (Y=0)
    if (WiFi.status() == WL_CONNECTED) {   // WIFI SUDAH TERHUBUNG?
      oled.print("WiFi: OK ");             // STATUS WIFI TERHUBUNG
      oled.print(WiFi.RSSI());             // KUAT SINYAL DALAM dBm
      oled.print("dBm");
      oled.setCursor(0, 8);                // BARIS 2 (Y=8)
      oled.print("IP:");                   // LABEL ALAMAT IP
      oled.print(WiFi.localIP());          // ALAMAT IP -> BUKA DI BROWSER
      oled.setCursor(0, 16);               // BARIS 3 (Y=16)
      oled.print("SSID:");                 // LABEL NAMA WIFI
      oled.print(WiFi.SSID());             // NAMA WIFI YANG TERSAMBUNG
      oled.setCursor(0, 24);               // BARIS 4 (Y=24)
      oled.print("Buka IP di browser");    // PETUNJUK
    } else {                               // WIFI BELUM / TIDAK TERHUBUNG
      oled.print("WiFi: MENCARI...");      // STATUS WIFI SEDANG MENCARI
      oled.setCursor(0, 8);                // BARIS 2 (Y=8)
      oled.print(WIFI_SSID);               // NAMA WIFI YANG DITUJU
    }
  } else {                                 // ===== HALAMAN 2: PESAN =====
    oled.setCursor(0, 0);                  // BARIS 1 (Y=0)
    oled.print("ATmega: ");                // LABEL PESAN DARI ATMEGA
    oled.print(potong(pesanAtmega));       // PESAN TERAKHIR DARI ATMEGA
    oled.setCursor(0, 8);                  // BARIS 2 (Y=8)
    oled.print("Balas : ");                // LABEL BALASAN KE ATMEGA
    oled.print(potong(pesanBalas));        // BALASAN TERAKHIR DARI ESP32
    oled.setCursor(0, 16);                 // BARIS 3 (Y=16)
    oled.print("Web   : ");                // LABEL PESAN DARI BROWSER
    oled.print(potong(pesanWeb));          // PESAN TERAKHIR DARI WEB
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

  server.on("/", handleRoot);              // DAFTARKAN HANDLER HALAMAN UTAMA
  server.on("/data", handleData);          // DAFTARKAN HANDLER DATA TERBARU
  server.on("/kirim", handleKirim);        // DAFTARKAN HANDLER KIRIM PESAN KE ATMEGA

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
    Serial.print("WiFi terhubung, buka: http://"); // INFO DI SERIAL MONITOR
    Serial.println(WiFi.localIP());        // TAMPILKAN IP UNTUK DIBUKA DI BROWSER
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

  // --- BAGIAN WEB SERVER: MULAI SETELAH WIFI TERHUBUNG, LAYANI PERMINTAAN BROWSER ---
  if (wifiOk && !serverJalan) {            // WIFI SUDAH ADA TAPI SERVER BELUM JALAN?
    server.begin();                        // MULAI WEB SERVER
    serverJalan = true;                    // TANDAI SUDAH JALAN
    Serial.println("Web server dimulai");  // INFO DI SERIAL MONITOR
  }
  if (serverJalan) {                       // SERVER SUDAH JALAN?
    server.handleClient();                 // PROSES PERMINTAAN DARI BROWSER (WAJIB DIPANGGIL BERULANG)
  }

  // --- BAGIAN UART: TERIMA DARI ATMEGA, BALAS ---
  if (Serial2.available()) {               // ADA DATA MASUK DARI ATMEGA?
    String data = Serial2.readStringUntil('\n'); // BACA SAMPAI KARAKTER ENTER
    data.trim();                           // HAPUS SPASI / '\r' / '\n' DI UJUNG TEKS
    Serial.print("TERIMA : ");             // TAMPILKAN DI SERIAL MONITOR
    Serial.println(data);

    pesanAtmega = data;                    // SIMPAN PESAN UNTUK OLED DAN WEB
    totalPesan++;                          // NAIKKAN COUNTER PESAN
    waktuPesanBaru = millis();             // CATAT WAKTU -> OLED PINDAH KE HALAMAN PESAN

    if (data == "Hello World!") {          // CEK ISI PESAN SAMA PERSIS DENGAN "Hello World!"?
      Serial2.println("Hello There!");     // YA -> BALAS "Hello There!" + ENTER KE ATMEGA
      pesanBalas = "Hello There!";         // SIMPAN BALASAN UNTUK OLED DAN WEB
      Serial.println("BALAS  : Hello There!"); // TAMPILKAN BALASAN DI SERIAL MONITOR
    } else {                               // PESAN LAIN (MISAL "OK: LED ON")
      pesanBalas = "-";                    // TIDAK ADA BALASAN
    }
    updateOLED();                          // LANGSUNG UPDATE OLED SAAT ADA PESAN BARU
  }

  // --- BAGIAN OLED: REFRESH BERKALA (GANTI HALAMAN & UPDATE RSSI) ---
  if (millis() - waktuOLED >= 500) {       // SUDAH 0,5 DETIK SEJAK UPDATE TERAKHIR?
    waktuOLED = millis();                  // CATAT WAKTU UPDATE SEKARANG
    updateOLED();                          // PERBARUI TAMPILAN OLED
  }
}
