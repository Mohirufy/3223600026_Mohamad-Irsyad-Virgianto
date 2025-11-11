# 🧩 ESP32-S3 Multitasking Peripheral Control

Proyek ini menampilkan implementasi **multitasking FreeRTOS** pada **ESP32-S3** untuk mengontrol berbagai periferal seperti OLED, LED, buzzer, tombol, potensiometer, rotary encoder, motor stepper, dan servo.

---

## 🔧 Hardware yang Digunakan
| Komponen | Deskripsi | Pin |
|-----------|------------|------|
| OLED | I2C 128x64 | SDA=8, SCL=9 |
| LED | Indikator | GPIO 7 |
| Buzzer | Aktif/Nonaktif | GPIO 6 |
| Tombol 1 | Input digital | GPIO 5 |
| Tombol 2 | Input digital | GPIO 4 |
| Potensiometer | Analog | GPIO 1 |
| Encoder | CLK=10, DT=11 |  |
| Stepper Motor | 4-pin | IN1=18, IN2=17, IN3=16, IN4=15 |
| Servo | PWM | GPIO 42 |

---

## Library yang Diperlukan
Pastikan kamu sudah menginstal library berikut di Arduino IDE:
- `Adafruit SSD1306`
- `Adafruit GFX`
- `AccelStepper`
- `ESP32Servo`
