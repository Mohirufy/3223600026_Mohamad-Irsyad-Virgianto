#include <Arduino.h>

#define POT_PIN 18
#define CORE_TO_RUN 1  // Ubah jadi 0 atau 1

void potTask(void *pvParameters) {
  int core = xPortGetCoreID();
  
  Serial.println("Potentiometer started on Core " + String(core));
  
  for(;;) {
    int val = analogRead(POT_PIN);
    
    if(core == 0) {
      // Core 0: Tampilkan raw value
      Serial.println("CORE 0 - VALUE: " + String(val));
    } else {
      // Core 1: Tampilkan persentase
      int percent = map(val, 0, 4095, 0, 100);
      Serial.println("CORE 1 - PERCENT: " + String(percent) + "%");
    }
    
    delay(1000); // Baca setiap 1 detik
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.print("Starting potentiometer on Core ");
  Serial.println(CORE_TO_RUN);
  
  xTaskCreatePinnedToCore(potTask, "Potentiometer", 2048, NULL, 1, NULL, CORE_TO_RUN);
}

void loop() {
  delay(5000);
}