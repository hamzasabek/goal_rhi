#include <Arduino.h>
#include "config.h"
#include "logging.h"

void setup() {
  initLogging();
  LOG("hello from Lab 0.2-F");
}

void loop() {
  static uint32_t last = 0;
  uint32_t now = millis();
  if (now - last >= 1000) {
    last = now;
    LOG("tick");
  }
}
