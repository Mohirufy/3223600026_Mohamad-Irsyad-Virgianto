#include <Arduino.h>
#include <AccelStepper.h>

#define IN1 18
#define IN2 17
#define IN3 16
#define IN4 15

#define CORE_ID 1

AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);

void taskStepper(void *pvParameters) {
  stepper.setMaxSpeed(400.0);
  stepper.setAcceleration(200.0);
  stepper.moveTo(200);

  while (1) {
    if (stepper.distanceToGo() == 0) {
      long tgt = (stepper.currentPosition() >= 0) ? -200 : 200;
      stepper.moveTo(tgt);
      Serial.print("Core: ");        Serial.println(xPortGetCoreID());
    }
    stepper.run();      
    vTaskDelay(1);       
  }
}

void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(taskStepper, "taskStepper", 4096, NULL, 1, NULL, CORE_ID);
}

void loop() {}