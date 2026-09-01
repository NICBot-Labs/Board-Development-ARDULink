ARDULink adalah Development Board Microcontroller berbasis ESP32 yang dipadukan dengan  chip tambahan ATmega328,board ini dirancang khusus untuk pembelajaran, riset, serta pengembangan proyek Internet of Things (IoT) dan sistem tertanam (embedded system) di lingkungan Universitas Nahdlatul Ulama Blitar maupun lainya

Fitur Utama ArduLink

1. Sistem Dual Mikrokontroler dengan total  3 Core
* Menggabungkan **ESP32 (Dual-Core)** dan **ATmega328P (Single-Core)** dalam satu PCB.
* **Kelebihan:** Memungkinkan pembagian tugas secara efisien (*multitasking*). ESP32 fokus menangani tugas berat seperti pemrosesan data, konektivitas Wi-Fi/Bluetooth, dan komunikasi *cloud*, sedangkan ATmega328P menangani pembacaan sensor atau kontrol aktuator secara *real-time* yang stabil dan responsif tanpa terganggu beban jaringan.

2. Sensor Suhu Internal LM35
* Dapat digunakan untuk memantau suhu operasional *board*, suhu lingkungan sekitar, atau sebagai data acuan awal (*baseline data*) dalam pengujian sistem termal.

3. Pewaktu Presisi Tinggi dengan RTC DS3231
* **Fungsinya** Menyediakan data waktu secara presisi (detik, menit, jam, tanggal) dengan baterai cadangan. Sangat berguna untuk *data logging*, pemicu jadwal otomatis (*timer/scheduling*), dan menjaga sinkronisasi waktu saat daya utama mati.

4. Sistem Monitoring Daya
* Dapat memantau kapasitas baterai atau level tegangan dari sumber listrik utama secara *real-time* untuk mencegah penurunan daya mendadak dan menjaga keamanan operasional sistem.

5. Indikator Visual Dual LED RGB
* **Fungsinya** Digunakan sebagai indikator status sistem multi-warna yang interaktif (status koneksi jaringan, indikator peringatan/error, atau mode operasi *board*).

6. Layar Tampilan OLED 0.91 Inci
* **Fungsinya** Digunakan untuk menampilkan informasi penting secara ringkas dan *real-time*, seperti nilai data sensor, status Wi-Fi/Bluetooth, IP address, atau menu navigasi tanpa memerlukan monitor eksternal.

7. Konektor Antena Eksternal
* **Keunggulannya** Meningkatkan stabilitas, daya tangkap, dan jangkauan sinyal Wi-Fi serta Bluetooth secara signifikan, cocok untuk penerapan di area terbuka (*outdoor*) atau lingkungan industri.

8. Komunikasi Antar-Mikrokontroler via UART
* Dirancang khusus agar ESP32 dan ATmega328P dapat saling bertukar data secara langsung melalui antarmuka serial, menciptakan satu kesatuan sistem yang utuh, tangguh, dan andal.

9. Kapasitas Ekspansi Luas engan total 43 GPIO
* **Keunggulannya** Menyediakan total **43 pin GPIO** yang fleksibel untuk dihubungkan ke berbagai macam sensor, modul, relay, dan aktuator eksternal dalam skala proyek yang besar dan kompleks.

10. Dual Port USB Type-C
* Dilengkapi dua port Type-C terpisah yang memudahkan proses *upload* program, pemrograman paralel, maupun *debugging* pada masing-masing mikrokontroler secara independen.
"""
