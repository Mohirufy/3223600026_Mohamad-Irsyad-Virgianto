#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>
#include <Wire.h>

// ======================= PIN DEFINITIONS =======================
#define SDA_PIN 8
#define SCL_PIN 9
#define LED_PIN 7
#define BUZZER_PIN 6
#define BTN1_PIN 5
#define BTN2_PIN 4
#define POT_PIN 1
#define ENCODER_CLK 10
#define ENCODER_DT 11
#define IN1 18
#define IN2 17
#define IN3 16
#define IN4 15
#define SERVO_PIN 42

// ======================= PERIPHERAL OBJECTS =======================
Adafruit_SSD1306 display(128, 64, &Wire, -1);
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);
Servo servo;

// ======================= GLOBAL VARIABLES =======================
volatile int encoderPos = 0;
int lastClkState = HIGH;

// ======================= TASK HANDLES =======================
TaskHandle_t oledTaskHandle;
TaskHandle_t ledTaskHandle;
TaskHandle_t buzzerTaskHandle;
TaskHandle_t buttonTaskHandle;
TaskHandle_t potTaskHandle;
TaskHandle_t encoderTaskHandle;
TaskHandle_t stepperTaskHandle;
TaskHandle_t servoTaskHandle;

// ======================= ENCODER ISR =======================
void IRAM_ATTR readEncoder() {
  int clkState = digitalRead(ENCODER_CLK);
  int dtState = digitalRead(ENCODER_DT);
  
  if (clkState != lastClkState) {
    if (dtState != clkState) {
      encoderPos++;
    } else {
      encoderPos--;
    }
  }
  lastClkState = clkState;
}

// ======================= CORE 0 TASKS =======================

// TASK: OLED DISPLAY (Core 0 - Medium Priority)
void oledTask(void *pvParameters) {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  unsigned long lastUpdate = 0;
  
  for(;;) {
    if (millis() - lastUpdate >= 200) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("CORE 0: OLED Task");
      display.println("-----------------");
      display.println("Hello World");
      display.println("-----------------");
      display.print("Core: ");
      display.println(xPortGetCoreID());
      display.display();
      lastUpdate = millis();
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// TASK: LED CONTROL (Core 0 - Low Priority)
void ledTask(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT);
  
  for(;;) {
    digitalWrite(LED_PIN, LOW);
    delay(1000);
    digitalWrite(LED_PIN, HIGH);
    
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// TASK: BUZZER CONTROL (Core 0 - Low Priority)
void buzzerTask(void *pvParameters) {
  pinMode(BUZZER_PIN, OUTPUT);
  
  for(;;) {
    noTone(BUZZER_PIN);
    delay(5000);
    tone(BUZZER_PIN, 1000, 100);
  }
}

// TASK: SERVO MOTOR (Core 0 - Medium Priority)
void servoTask(void *pvParameters) {
  servo.attach(SERVO_PIN);
  
  int pos = 0;
  bool direction = true;
  unsigned long lastSweep = 0;
  
  for(;;) {
    if (millis() - lastSweep > 15) {
      if (direction) {
        pos++;
        if (pos >= 180) {
          direction = false;
        }
      } else {
        pos--;
        if (pos <= 0) {
          direction = true;
        }
      }
      servo.write(pos);
      lastSweep = millis();
    }
    
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

// ======================= CORE 1 TASKS =======================

// TASK: STEPPER MOTOR (Core 1 - High Priority)
void stepperTask(void *pvParameters) {
  stepper.setMaxSpeed(800);
  stepper.setAcceleration(400);
  stepper.setCurrentPosition(0);
  
  int targetPos = 0;
  bool direction = true;
  unsigned long lastMove = 0;
  
  for(;;) {
    // Continuous back and forth movement
    if (millis() - lastMove > 2000) {
      if (direction) {
        targetPos += 1000;
      } else {
        targetPos -= 1000;
      }
      stepper.moveTo(targetPos);
      direction = !direction;
      lastMove = millis();
    }
    
    stepper.run();
    vTaskDelay(pdMS_TO_TICKS(1)); // High frequency for smooth movement
  }
}

// TASK: ROTARY ENCODER (Core 1 - High Priority)
void encoderTask(void *pvParameters) {
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  
  lastClkState = digitalRead(ENCODER_CLK);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), readEncoder, CHANGE);
  
  int lastPos = 0;
  unsigned long lastPrint = 0;
  
  for(;;) {
    // Monitor encoder changes
    if (encoderPos != lastPos) {
      lastPos = encoderPos;
    }
    
    // Print position every 2 seconds for debugging
    if (millis() - lastPrint > 2000) {
      Serial.printf("Core %d - Encoder: %d\n", xPortGetCoreID(), encoderPos);
      lastPrint = millis();
    }
    
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// TASK: BUTTON INPUT (Core 1 - Medium Priority)
void buttonTask(void *pvParameters) {
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);
  
  int lastBtn1State = HIGH;
  int lastBtn2State = HIGH;
  
  for(;;) {
    int btn1State = digitalRead(BTN1_PIN);
    int btn2State = digitalRead(BTN2_PIN);
    
    // Simple button press detection - active LOW
    if (btn1State == LOW && lastBtn1State == HIGH) {
      Serial.println("✅ BUTTON 1 PRESSED");
      delay(50); // Simple debounce
    }
    
    if (btn2State == LOW && lastBtn2State == HIGH) {
      Serial.println("✅ BUTTON 2 PRESSED");
      delay(50); // Simple debounce
    }
    
    lastBtn1State = btn1State;
    lastBtn2State = btn2State;
    
    vTaskDelay(pdMS_TO_TICKS(10)); // Check every 10ms
  }
}

// TASK: POTENTIOMETER (Core 1 - Low Priority)
void potTask(void *pvParameters) {
  int lastRawValue = 0;
  unsigned long lastPrint = 0;
  
  for(;;) {
    int rawValue = analogRead(POT_PIN);
    
    // Filter small changes and print occasionally
    if (abs(rawValue - lastRawValue) > 10) {
      lastRawValue = rawValue;
    }
    
    // Print value every 3 seconds for debugging
    if (millis() - lastPrint > 3000) {
      Serial.printf("Core %d - Potentiometer: %d\n", xPortGetCoreID(), rawValue);
      lastPrint = millis();
    }
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// ======================= SETUP =======================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== ESP32-S3 Dual Core Task Allocation ===");
  Serial.println("Core 0 Tasks: OLED, LED, Buzzer, Servo");
  Serial.println("Core 1 Tasks: Stepper, Encoder, Button, Potentiometer");
  
  // ======================= CORE 0 TASKS =======================
  xTaskCreatePinnedToCore(
    oledTask,        // Task function
    "OLED_Task",     // Task name
    4096,            // Stack size
    NULL,            // Parameters
    2,               // Priority (Medium)
    &oledTaskHandle, // Task handle
    0                // Core 0
  );
  
  xTaskCreatePinnedToCore(
    ledTask,
    "LED_Task",
    2048,
    NULL,
    1,               // Priority (Low)
    NULL,
    0                // Core 0
  );
  
  xTaskCreatePinnedToCore(
    buzzerTask,
    "Buzzer_Task",
    2048,
    NULL,
    1,               // Priority (Low)
    NULL,
    0                // Core 0
  );
  
  xTaskCreatePinnedToCore(
    servoTask,
    "Servo_Task",
    2048,
    NULL,
    2,               // Priority (Medium)
    NULL,
    0                // Core 0
  );
  
  // ======================= CORE 1 TASKS =======================
  xTaskCreatePinnedToCore(
    stepperTask,
    "Stepper_Task",
    4096,
    NULL,
    4,               // Priority (High)
    &stepperTaskHandle,
    1                // Core 1
  );
  
  xTaskCreatePinnedToCore(
    encoderTask,
    "Encoder_Task",
    2048,
    NULL,
    3,               // Priority (High)
    NULL,
    1                // Core 1
  );
  
  xTaskCreatePinnedToCore(
    buttonTask,
    "Button_Task",
    2048,
    NULL,
    2,               // Priority (Medium)
    NULL,
    1                // Core 1
  );
  
  xTaskCreatePinnedToCore(
    potTask,
    "Pot_Task",
    2048,
    NULL,
    1,               // Priority (Low)
    NULL,
    1                // Core 1
  );
  
  Serial.println("All tasks created and allocated to cores!");
  Serial.println("System running...");
  
  vTaskDelete(NULL); // Delete setup task
}

void loop() {
}