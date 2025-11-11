#include <Arduino.h>
#include <ESP32Servo.h>

#define SERVO_PIN 5

// Pilih core yang diinginkan: 0 atau 1
#define CORE_TO_RUN 1  // Ganti dengan 0 atau 1

Servo myServo;

// Task untuk Servo dengan movement pattern berbeda berdasarkan core
void servoTask(void *pvParameters) {
  int coreID = xPortGetCoreID();
  Serial.println(coreID);

  myServo.attach(SERVO_PIN);
  int movementCount = 0;
  int pos = 0;

  for(;;) {
    movementCount++;
    
    if(coreID == 0) {
      // Pattern untuk Core 0 - Sweep otomatis
      Serial.println("Core 0: SWEEP Pattern - Movement " + String(movementCount));
      
      // Sweep dari 0° ke 180°
      for(pos = 0; pos <= 180; pos += 2) {
        myServo.write(pos);
        vTaskDelay(15 / portTICK_PERIOD_MS);
      }
      
      // Sweep dari 180° ke 0°
      for(pos = 180; pos >= 0; pos -= 2) {
        myServo.write(pos);
        vTaskDelay(15 / portTICK_PERIOD_MS);
      }
      
    } else {
      // Pattern untuk Core 1 - Gerakan spesifik
      Serial.println("Core 1: SPECIFIC Pattern - Movement " + String(movementCount));
      
      // Gerakan 0° -> 90° -> 180° -> 90° -> 0°
      myServo.write(0);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      
      myServo.write(90);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      
      myServo.write(180);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      
      myServo.write(90);
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Jeda antar pattern
  }
}

void setup() {
  Serial.begin(115200);
  vTaskDelay(3000 / portTICK_PERIOD_MS);

  Serial.println("=== SINGLE SERVO CORE SELECTION ===");
  Serial.println("Servo PIN: " + String(SERVO_PIN));
  Serial.println("Selected Core: " + String(CORE_TO_RUN));
  
  if(CORE_TO_RUN == 0) {
    Serial.println("Pattern: AUTO SWEEP (0° to 180°)");
  } else {
    Serial.println("Pattern: SPECIFIC MOVEMENT (0°-90°-180°-90°)");
  }

  // Buat task di core yang dipilih
  xTaskCreatePinnedToCore(
      servoTask,
      "Servo_Task",
      2048,
      NULL,
      1,
      NULL,
      CORE_TO_RUN  // Core dipilih dari define
  );

  Serial.println("Servo task created successfully!");
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}