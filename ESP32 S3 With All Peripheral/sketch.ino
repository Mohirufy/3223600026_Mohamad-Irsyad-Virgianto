#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>

// ======================= OLED =======================
#define SDA_PIN 8
#define SCL_PIN 9
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ======================= LED & BUZZER =======================
#define LED_PIN 7
#define BUZZER_PIN 6

// ======================= BUTTON =======================
#define BTN1_PIN 5
#define BTN2_PIN 4

// ======================= POTENTIOMETER =======================
#define POT_PIN 1

// ======================= ROTARY ENCODER =======================
#define ENCODER_CLK 10
#define ENCODER_DT  11
volatile int encoderPos = 0;
int lastClkState;

// ======================= STEPPER =======================
#define IN1 18
#define IN2 17
#define IN3 16
#define IN4 15
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);

// ======================= SERVO =======================
#define SERVO_PIN 42
Servo servo;

// ======================= RTOS HANDLES =======================
TaskHandle_t TaskCore0;
TaskHandle_t TaskCore1;

// ======================= TASK 1: I/O + OLED =======================
void TaskCore0code(void *pvParameters) {
  (void) pvParameters;

  Wire.begin(SDA_PIN, SCL_PIN);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED gagal diinisialisasi!");
    vTaskDelete(NULL);
  }

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);

  lastClkState = digitalRead(ENCODER_CLK);
  Serial.println("Task Core0 berjalan...");

  for (;;) {
    int potValue = analogRead(POT_PIN);
    int btn1 = digitalRead(BTN1_PIN);
    int btn2 = digitalRead(BTN2_PIN);

    int clkState = digitalRead(ENCODER_CLK);
    if (clkState != lastClkState) {
      if (digitalRead(ENCODER_DT) != clkState) {
        encoderPos++;
      } else {
        encoderPos--;
      }
    }
    lastClkState = clkState;

    digitalWrite(LED_PIN, btn1 == LOW);
    digitalWrite(BUZZER_PIN, btn2 == LOW);

    // Tampilkan data di OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("ESP32-S3 RTOS Test");
    display.setCursor(0, 12);
    display.print("Pot: ");
    display.print(potValue);
    display.setCursor(0, 24);
    display.print("Enc: ");
    display.print(encoderPos);
    display.setCursor(0, 36);
    display.print("BTN1: ");
    display.print(btn1 == LOW ? "Pressed" : "Open");
    display.setCursor(0, 48);
    display.print("BTN2: ");
    display.print(btn2 == LOW ? "Pressed" : "Open");
    display.display();

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// ======================= TASK 2: STEPPER + SERVO =======================
void TaskCore1code(void *pvParameters) {
  (void) pvParameters;

  stepper.setMaxSpeed(400.0);
  stepper.setAcceleration(100.0);

  servo.attach(SERVO_PIN);
  Serial.println("Task Core1 berjalan...");

  for (;;) {
    // Servo bergerak dari 0 ke 180 dan kembali
    for (int pos = 0; pos <= 180; pos += 5) {
      servo.write(pos);
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    for (int pos = 180; pos >= 0; pos -= 5) {
      servo.write(pos);
      vTaskDelay(20 / portTICK_PERIOD_MS);
    }

    // Stepper jalan bolak-balik
    stepper.moveTo(200);
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
    stepper.moveTo(-200);
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
  }
}

// ======================= SETUP =======================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Inisialisasi...");

  // Buat Task di dua core
  xTaskCreatePinnedToCore(
    TaskCore0code, "TaskCore0",
    10000, NULL, 1, &TaskCore0, 0);

  xTaskCreatePinnedToCore(
    TaskCore1code, "TaskCore1",
    10000, NULL, 1, &TaskCore1, 1);
}

// ======================= LOOP =======================
void loop() {
  vTaskDelete(NULL); // tidak digunakan, semua dihandle oleh task
}
