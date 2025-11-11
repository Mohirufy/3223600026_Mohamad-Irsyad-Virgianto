#include <Arduino.h>

#define CORE_0 0
#define CORE_1 1

#define BUTTON_1_PIN 1
#define BUTTON_2_PIN 37

int button1Count = 0;
int button2Count = 0;

// Task Button 1 di Core 0
void button1Task(void *pvParameters) {
  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  
  for(;;) {
    if(digitalRead(BUTTON_1_PIN) == LOW) {
      button1Count++;
      Serial.println("Button 1 pressed! Count: " + String(button1Count) + " on Core 0");
      delay(200); // Simple debounce
      while(digitalRead(BUTTON_1_PIN) == LOW) { delay(10); } // Wait release
    }
    delay(10);
  }
}

// Task Button 2 di Core 1
void button2Task(void *pvParameters) {
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  
  for(;;) {
    if(digitalRead(BUTTON_2_PIN) == LOW) {
      button2Count++;
      Serial.println("Button 2 pressed! Count: " + String(button2Count) + " on Core 1");
      delay(200); // Simple debounce
      while(digitalRead(BUTTON_2_PIN) == LOW) { delay(10); } // Wait release
    }
    delay(10);
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("Dual Button FreeRTOS Test");
  Serial.println("Button 1 -> Core 0");
  Serial.println("Button 2 -> Core 1");

  xTaskCreatePinnedToCore(button1Task, "BTN1", 1024, NULL, 1, NULL, CORE_0);
  xTaskCreatePinnedToCore(button2Task, "BTN2", 1024, NULL, 1, NULL, CORE_1);
}

void loop() {
  delay(1000);
}