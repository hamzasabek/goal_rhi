#pragma once
#include <Arduino.h>
#include "config.h"

// Driver servos via PCA9685 — squelette "no-op" tant qu'on n'a pas le matériel
class ServoBus {
public:
  bool begin() {
    // plus tard: init I2C + PCA9685 à PCA_PWM_FREQ_HZ
    _ready = true;
    return true;
  }

  // Ecrire une impulsion en microsecondes sur un canal (ici on stocke juste)
  void writeUS(uint8_t ch, uint16_t us) {
    // plus tard: us -> ticks & pca.setPWM(ch, 0, ticks)
    if (ch < 16) _last[ch] = us;  // mémorise pour debug
  }

  // Centrer 3 servos selon la config
  void center3() {
    writeUS(SERVO_CH_0, SERVO0_US_NEU);
    writeUS(SERVO_CH_1, SERVO1_US_NEU);
    writeUS(SERVO_CH_2, SERVO2_US_NEU);
  }

  bool ready() const { return _ready; }
  uint16_t lastUS(uint8_t ch) const { return (ch < 16) ? _last[ch] : 0; }

private:
  bool _ready = false;
  uint16_t _last[16] = {0}; // mémorise la dernière valeur envoyée par canal
};
