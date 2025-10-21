#pragma once
#include <Arduino.h>
#include "config.h"

// Démarre la console série et imprime une bannière propre
inline void initLogging() {
  Serial.begin(SERIAL_BAUD);
  uint32_t t0 = millis();
  while (!Serial && (millis() - t0) < 2000) { /* attendre console max 2s */ }
  Serial.println();
  Serial.print(F(PROJECT_NAME " "));
  Serial.println(F(PROJECT_VERSION));
}

// Préfixe horodaté "mm:ss.mmm > "
inline void log_ts_prefix() {
  uint32_t ms = millis();
  uint32_t s  = ms / 1000;  ms %= 1000;
  uint32_t m  = s / 60;     s  %= 60;
  char buf[16];
  snprintf(buf, sizeof(buf), "%02lu:%02lu.%03lu",
           (unsigned long)m, (unsigned long)s, (unsigned long)ms);
  Serial.print(buf);
  Serial.print(" > ");
}

// Macros simples de log
#define LOG(msg)            do { log_ts_prefix(); Serial.println(msg); } while(0)
#define LOGF(fmt, ...)      do { log_ts_prefix(); Serial.printf((fmt), __VA_ARGS__); Serial.println(); } while(0)
