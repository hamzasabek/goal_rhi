#include <Arduino.h>
#include "config.h"

void setup() {
  Serial.begin(SERIAL_BAUD);
  // pas d'accès hardware ici — juste un ping sur la console plus tard
}

void loop() { }
