#include <Arduino.h>

#define BUZZER_PIN 1
#define CORE_TO_RUN 1  // Ubah jadi 0 atau 1

void buzzerTask(void *pvParameters) {
  int core = xPortGetCoreID();
  pinMode(BUZZER_PIN, OUTPUT);
  
  Serial.println("Buzzer started on Core " + String(core));
  
  for(;;) {
    if(core == 0) {
      // Core 0: Siren
      Serial.println("CORE 0: Siren sound");
      for(int i=200; i<=1000; i+=10) tone(BUZZER_PIN, i), delay(10);
      for(int i=1000; i>=200; i-=10) tone(BUZZER_PIN, i), delay(10);
    } else {
      // Core 1: Bell
      Serial.println("CORE 1: Bell sound");
      tone(BUZZER_PIN, 800); delay(300);
      noTone(BUZZER_PIN); delay(200);
      tone(BUZZER_PIN, 800); delay(300);
    }
    noTone(BUZZER_PIN);
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.print("Starting buzzer on Core ");
  Serial.println(CORE_TO_RUN);
  
  xTaskCreatePinnedToCore(buzzerTask, "Buzzer", 1024, NULL, 1, NULL, CORE_TO_RUN);
}

void loop() {
  delay(5000);
}