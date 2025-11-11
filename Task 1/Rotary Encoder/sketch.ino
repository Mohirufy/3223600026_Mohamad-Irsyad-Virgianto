#include <Arduino.h>

// ====== KONFIG ======
#define CORE_ID 1
#define ENCODER_CLK 10
#define ENCODER_DT  11

volatile long encoderPos = 0;
volatile uint32_t lastMicrosISR = 0;

void IRAM_ATTR onClkChange() {
  uint32_t now = micros();
  if (now - lastMicrosISR < 1000) return; // debounce ~1ms
  lastMicrosISR = now;

  int clk = digitalRead(ENCODER_CLK);
  int dt  = digitalRead(ENCODER_DT);
  // A/B Gray code: arah ditentukan oleh relasi CLK vs DT
  if (clk == dt) encoderPos++; else encoderPos--;
}

TaskHandle_t taskH;

void encoderTask(void *pv) {
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT,  INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), onClkChange, CHANGE);

  for (;;) {
    static long last = LONG_MIN;
    long cur = encoderPos;
    if (cur != last) {
      Serial.printf("Encoder: %ld\n", cur);
      last = cur;
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(encoderTask, "encTask", 4096, NULL, 1, &taskH, CORE_ID);
}
void loop() { vTaskDelay(portMAX_DELAY); }