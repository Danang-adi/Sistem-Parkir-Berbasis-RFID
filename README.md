# 🚗 Smart Parking System IoT (ESP8266)

![Status](https://img.shields.io/badge/Status-Prototype-green)
![Platform](https://img.shields.io/badge/Platform-NodeMCU%20ESP8266-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B%20%2F%20Arduino-orange)

Sistem parkir pintar berbasis IoT menggunakan **NodeMCU ESP8266** yang mengintegrasikan sistem gerbang otomatis dan monitoring ketersediaan slot parkir secara real-time.

---

## 🛠️ Fitur Utama

1.  **Sistem Masuk (Entry System):** Menggunakan **RFID (RC522)**. Palang pintu hanya terbuka jika kartu terdeteksi valid.
2.  **Sistem Keluar (Exit System):** Menggunakan **Sensor Ultrasonik**. Palang terbuka otomatis ketika kendaraan terdeteksi ingin keluar (tanpa tap kartu).
3.  **Monitoring Slot (Slot Tracking):** Menggunakan **Sensor Infrared** yang diperluas dengan **PCF8574**. Mendeteksi sisa slot parkir dan menampilkannya di LCD.
4.  **Informasi Real-time:** Menampilkan status akses dan sisa slot pada **LCD 16x2**.

---

## ⚙️ Cara Kerja (Workflow)

### 1. Pintu Masuk (Entry)
* Pengguna menempelkan kartu pada **RFID Reader**.
* Jika ID kartu valid, **Servo Masuk** akan bergerak 90° (Membuka).
* Sistem mengurangi jumlah slot parkir yang tersedia.

### 2. Monitoring Slot
* **Sensor Infrared** yang terpasang di setiap slot parkir mendeteksi keberadaan mobil.
* Data dikirim ke NodeMCU melalui modul ekspansi **PCF8574** (I2C).
* Sisa slot dihitung dan ditampilkan di layar LCD.

### 3. Pintu Keluar (Exit)
* **Sensor Ultrasonik** mendeteksi objek (mobil) pada jarak tertentu (misal: < 10cm).
* Jika terdeteksi, **Servo Keluar** otomatis bergerak 90° (Membuka).
* Sistem menambah jumlah slot parkir yang tersedia.

---

## 📦 Daftar Komponen (Hardware List)

Berikut adalah komponen yang digunakan dalam proyek ini:

| No | Komponen | Deskripsi & Fungsi |
| :--- | :--- | :--- |
| 1 | **NodeMCU Lolin V3 (ESP8266)** | Otak utama sistem yang mengolah logika, membaca sensor RFID/Ultrasonik, dan mengontrol servo. |
| 2 | **RC522 RFID Reader** | Membaca ID kartu unik untuk verifikasi akses masuk. |
| 3 | **Tower Pro SG90 Servo (x2)** | Aktuator penggerak palang pintu masuk dan pintu keluar. |
| 4 | **HY-SRF05 Ultrasonic** | Mendeteksi keberadaan kendaraan di pintu keluar menggunakan gelombang suara. |
| 5 | **Infrared Barrier Sensor** | Mendeteksi apakah slot parkir terisi atau kosong (Active LOW). |
| 6 | **PCF8574 I/O Expander** | Menambah jumlah pin I/O via jalur I2C untuk menghubungkan banyak sensor IR. |
| 7 | **LCD 16x2 + I2C Backpack** | Menampilkan informasi sisa slot dan status akses (Diterima/Ditolak). |
| 8 | **LM2596 Step Down + Voltmeter** | Menurunkan tegangan adaptor 12V menjadi 5V/3.3V yang stabil untuk mikrokontroler & sensor. |
| 9 | **Adaptor 12V** | Sumber daya utama listrik. |
| 10 | **PCB Universal Double-Sided** | Media perakitan komponen agar lebih rapi dan kuat dibanding breadboard. |
| 11 | **Kabel Jumper (M-M, M-F, F-F)** | Menghubungkan antar modul sensor, mikrokontroler, dan PCB. |
| 12 | **Header Socket Female 2x8** | Konektor agar modul bisa dilepas-pasang dari PCB tanpa solder ulang. |

---

## 🔌 Konfigurasi Pin (Wiring)

Berikut adalah skema penyambungan kabel ke **NodeMCU ESP8266**:

### 1. Jalur I2C (LCD & PCF8574)
* **SDA**: GPIO 4 (D2)
* **SCL**: GPIO 5 (D1)

### 2. Jalur SPI (RFID RC522)
* **SDA (SS)**: GPIO 15 (D8)
* **SCK**: GPIO 14 (D5)
* **MOSI**: GPIO 13 (D7)
* **MISO**: GPIO 12 (D6)
* **RST**: GPIO 0 (D3)

### 3. Aktuator & Sensor Lain
* **Servo Masuk (RFID)**: GPIO 2 (D4)
* **Servo Keluar (Ultra)**: GPIO 16 (D0)
* **Ultrasonik Trigger**: GPIO 3 (RX)
* **Ultrasonik Echo**: GPIO 1 (TX)

---

## 💻 Instalasi & Penggunaan

### Prasyarat Software
Pastikan Anda telah menginstal **Arduino IDE** dan library berikut:
1.  `MFRC522` (oleh GithubCommunity)
2.  `Servo` (Built-in)
3.  `LiquidCrystal_I2C` (oleh Frank de Brabander)
4.  `PCF8574` (oleh Rob Tillaart)

### Langkah Upload
1.  Buka file `SistemParkir.ino` di Arduino IDE.
2.  Pilih Board: **NodeMCU 1.0 (ESP-12E Module)**.
3.  Pastikan kabel USB terhubung.
4.  **PENTING:** Cabut sementara kabel di pin **RX** dan **TX** (Sensor Ultrasonik) saat proses upload agar tidak error.
5.  Klik **Upload**.
6.  Pasang kembali kabel RX/TX setelah upload selesai.
