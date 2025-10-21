#include <Arduino.h>
#include "config.h"
#include "logging.h"
#include "drivers/imu.h"
#include "drivers/servo.h"

ImuDriver  imu;
ServoBus   servos;

void setup() {
  initLogging();
  LOG("drivers: setup");

  // IMU (squelette)
  imu.setAxisFromConfig();
  bool imu_ok = imu.begin();
  LOGF("IMU begin: %s", imu_ok ? "OK" : "FAIL");

  // Servos (squelette)
  bool pca_ok = servos.begin();
  LOGF("PCA begin: %s", pca_ok ? "OK" : "FAIL");

  // centrer (virtuellement) les 3 servos
  servos.center3();
  LOG("servos: centered (virtual)");
}

void loop() {
  // Boucle "compile-only": on simule la vie
  static uint32_t last = 0;
  uint32_t now = millis();
  if (now - last >= 1000) {
    last = now;
    imu.update(); // (ne fait rien pour l'instant)
    LOGF("heartbeat | theta_deg=%.2f | S0=%u S1=%u S2=%u",
         imu.angleDeg(),
         servos.lastUS(SERVO_CH_0),
         servos.lastUS(SERVO_CH_1),
         servos.lastUS(SERVO_CH_2));
  }
}
