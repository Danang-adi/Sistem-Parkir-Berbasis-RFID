#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <PCF8574.h>
#include <LiquidCrystal_I2C.h>

// --- KONFIGURASI PIN ---
#define SS_PIN  15  // D8
#define RST_PIN 0   // D3
#define SERVO_RFID_PIN 2   // D4 (Pintu Masuk)
#define SERVO_ULTRA_PIN 16 // D0 (Pintu Keluar)
#define TRIG_PIN 3  // RX (Hati-hati, cabut saat upload)
#define ECHO_PIN 1  // TX (Hati-hati, cabut saat upload)

// --- INISIALISASI OBJEK ---
PCF8574 pcf8574(0x20); 
LiquidCrystal_I2C lcd(0x27, 16, 2); 
MFRC522 rfid(SS_PIN, RST_PIN);
Servo servoRFID, servoUltra;

// --- VARIABEL GLOBAL ---
unsigned long lastActionTime = 0;
const int jedaAksi = 5000; // Jeda antar gerakan pintu (5 detik)

void setup() {
  // Gunakan Wire.begin(SDA, SCL) sesuai pin NodeMCU kamu
  Wire.begin(4, 5); 
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("SISTEM PARKIR");
  lcd.setCursor(0,1);
  lcd.print("LOADING...");

  // Inisialisasi SPI dan RFID
  SPI.begin();
  rfid.PCD_Init();
  
  // Inisialisasi PCF8574 (Sensor Infrared)
  pcf8574.begin();
  
  // Inisialisasi Ultrasonik
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // --- REVISI POSISI AWAL SERVO (TUTUP = 0 DERAJAT) ---
  servoRFID.attach(SERVO_RFID_PIN);
  servoRFID.write(0); // Posisi awal menutup (datar)
  delay(500);
  servoRFID.detach();

  servoUltra.attach(SERVO_ULTRA_PIN);
  servoUltra.write(0); // Posisi awal menutup (datar)
  delay(500);
  servoUltra.detach();

  delay(1000);
  lcd.clear();
}

void loop() {
  // --- AUTO-RECOVER RFID (Jika komunikasi macet) ---
  if (rfid.PCD_ReadRegister(rfid.VersionReg) == 0x00 || rfid.PCD_ReadRegister(rfid.VersionReg) == 0xFF) {
    rfid.PCD_Init(); 
  }

  // --- 1. CEK SISA SLOT (INFRARED via PCF8574) ---
  int val = pcf8574.read8();
  int terisi = 0;
  for (int i = 0; i < 4; i++) {
    if (bitRead(val, i) == LOW) terisi++; // Sensor IR aktif LOW
  }
  int sisaParkir = 4 - terisi;

  lcd.setCursor(0, 0);
  lcd.print("Sisa Slot: ");
  lcd.print(sisaParkir);
  lcd.print("   ");

  // --- 2. LOGIKA RFID (PINTU MASUK - SEMUA KARTU DITERIMA) ---
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (millis() - lastActionTime > jedaAksi) {
      if (sisaParkir > 0) {
        lcd.setCursor(0, 1);
        lcd.print("AKSES DITERIMA ");
        gerakkanServo(servoRFID, SERVO_RFID_PIN); // Panggil fungsi gerak
        lastActionTime = millis();
      } else {
        lcd.setCursor(0, 1);
        lcd.print("PARKIR PENUH!  ");
        delay(2000);
      }
    }
    // Tanpa PICC_HaltA agar bisa ditap terus menerus
  }

  // --- 3. LOGIKA ULTRASONIK (PINTU KELUAR) ---
  int jarak = bacaJarak();
  if (jarak > 1 && jarak < 10) { // Jika ada mobil jarak 2-10cm
    if (millis() - lastActionTime > jedaAksi) {
      lcd.setCursor(0, 1);
      lcd.print("MOBIL KELUAR...");
      gerakkanServo(servoUltra, SERVO_ULTRA_PIN); // Panggil fungsi gerak
      lastActionTime = millis();
    }
  }

  // Tampilan Standby jika tidak ada aksi
  if (millis() - lastActionTime > 3000) {
    lcd.setCursor(0, 1);
    lcd.print("SIAP SCAN KARTU");
  }

  delay(50); // Loop cepat agar responsif
}

// --- REVISI FUNGSI GERAK SERVO (0 ke 90 derajat) ---
void gerakkanServo(Servo &s, int pin) {
  s.attach(pin);      // Hubungkan sinyal
  
  // 1. MEMBUKA (Naik ke 90 derajat)
  s.write(90); 
  delay(1000);        // Waktu untuk servo bergerak naik
  
  // 2. TAHAN (Mobil lewat)
  delay(3000);        
  
  // 3. MENUTUP (Turun kembali ke 0 derajat)
  s.write(0); 
  delay(1000);        // Waktu untuk servo bergerak turun
  
  s.detach();         // Putus sinyal agar servo tidak bergetar/panas
}

// --- FUNGSI BACA JARAK ---
int bacaJarak() {
  digitalWrite(TRIG_PIN, LOW); 
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); 
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long durasi = pulseIn(ECHO_PIN, HIGH, 25000); // Timeout 25ms
  if (durasi <= 0) return 999;
  return durasi * 0.034 / 2;
}