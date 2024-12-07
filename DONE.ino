#include <LiquidCrystal_I2C.h>

// Inisialisasi LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns, 2 rows

// Pin untuk POMPA1
#define trigPin 9     // Pin trigger untuk sensor ultrasonik
#define echoPin 8     // Pin echo untuk sensor ultrasonik
#define relayPin1 3   // Pin relay untuk pompa 1

// Pin untuk POMPA2_LCD_PH_LED
const int ph_Pin = A0;   // Pin untuk sensor pH
const int relayPin2 = 4; // Pin untuk relay module pompa 2
const int ledRedPin = 2; // Pin untuk LED merah

// Variabel untuk sensor pH
float PH_step;
float Po;
int nilai_analog_PH;
double TeganganPh;

// Kalibrasi nilai pH (sesuaikan berdasarkan kalibrasi sensor)
float PH4 = 3.8;  // Tegangan pada larutan pH 4
float PH7 = 3.6;  // Tegangan pada larutan pH 7

void setup() {
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();

  // Inisialisasi pin untuk POMPA1
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin1, OUTPUT);

  // Inisialisasi pin untuk POMPA2_LCD_PH_LED
  pinMode(ph_Pin, INPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(ledRedPin, OUTPUT);

  // Pastikan semua perangkat dalam kondisi awal (nonaktif)
  digitalWrite(relayPin1, HIGH); // Pompa 1 mati
  digitalWrite(relayPin2, HIGH); // Pompa 2 mati
  digitalWrite(ledRedPin, LOW);  // LED merah mati

  // Serial monitor untuk debugging
  Serial.begin(9600);
}

void loop() {
  // Bagian POMPA1
  long duration, distance;

  // Mengukur jarak menggunakan sensor ultrasonik
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) * 0.0343; // Rumus untuk menghitung jarak dalam cm

  Serial.print("Jarak: ");
  Serial.println(distance);

  if (distance >= 0 && distance <= 8) {
    digitalWrite(relayPin1, LOW); // Menyalakan pompa 1
    Serial.println("Pompa 1 ON");
    delay(120000);                // Pompa 1 menyala selama 30 detik
    digitalWrite(relayPin1, HIGH); // Mematikan pompa 1
    Serial.println("Pompa 1 OFF");
  } else {
    digitalWrite(relayPin1, HIGH); // Pastikan pompa 1 mati jika jarak tidak sesuai
    Serial.println("Pompa 1 OFF (jarak tidak sesuai)");
  }

  // Bagian POMPA2_LCD_PH_LED (dijalankan setelah POMPA1 berhenti)
  nilai_analog_PH = analogRead(ph_Pin);
  TeganganPh = 5.0 / 1024.0 * nilai_analog_PH;

  // Kalkulasi nilai pH
  PH_step = (PH4 - PH7) / 3;
  Po = 7.00 + ((PH7 - TeganganPh) / PH_step);

  // Debugging nilai pH
  Serial.print("Nilai ADC Ph: ");
  Serial.println(nilai_analog_PH);
  Serial.print("Tegangan Ph: ");
  Serial.println(TeganganPh, 3);
  Serial.print("Nilai Ph cairan: ");
  Serial.println(Po, 2);

  // Tampilkan status di LCD
  lcd.clear();
  if (Po >= 6.0 && Po <= 8.0) {
    lcd.setCursor(0, 0);
    lcd.print("Air Aman");
    lcd.setCursor(0, 1);
    lcd.print("pH: ");
    lcd.print(Po, 2);

    // Hidupkan pompa 2
    digitalWrite(relayPin2, HIGH); // Relay pompa 2 aktif
    digitalWrite(ledRedPin, LOW);  // LED merah mati
    Serial.println("Pompa 2 ON, LED OFF");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Air Tdk Aman");
    lcd.setCursor(0, 1);
    lcd.print("pH: ");
    lcd.print(Po, 2);

    // Matikan pompa 2 dan nyalakan LED merah berkedip
    digitalWrite(relayPin2, LOW); // Relay pompa 2 mati
    Serial.println("Pompa 2 OFF");

    for (int i = 0; i < 6; i++) { // LED merah berkedip 6 kali
      digitalWrite(ledRedPin, HIGH);
      delay(250);
      digitalWrite(ledRedPin, LOW);
      delay(250);
    }
  }

  // Delay untuk pembacaan berikutnya
  delay(3000);
}
