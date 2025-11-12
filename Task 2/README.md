# ESP32-S3 Multitasking Peripheral Control

Menampilkan implementasi **multitasking FreeRTOS** pada **ESP32-S3** untuk mengontrol berbagai periferal seperti OLED, LED, buzzer, tombol, potensiometer, rotary encoder, motor stepper, dan servo.

Link Video: [Video Hasil Percobaan Semua Peripheral](https://drive.google.com/file/d/1gZBGjFDgAgjhzbgmneUae_K-EvhZoiEe/view?usp=sharing) 

---

## Hardware yang Digunakan
| Komponen | Pin |
|-----------|------------|------|
| OLED | SDA=8, SCL=9 |
| LED | GPIO 7 |
| Buzzer | GPIO 6 |
| Tombol 1 | GPIO 5 |
| Tombol 2 | GPIO 4 |
| Potensiometer | GPIO 1 |
| Encoder | CLK=10, DT=11 |
| Stepper Motor | IN1=18, IN2=17, IN3=16, IN4=15 |
| Servo | GPIO 42 |

---

## Library yang Diperlukan
- `Adafruit SSD1306`
- `Adafruit GFX`
- `AccelStepper`
- `ESP32Servo`
