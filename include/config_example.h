#pragma once

// ===== Identité du build =====
#define PROJECT_NAME    "GOAL_RHI"
#define PROJECT_VERSION "v0.2-lab0.2D"

// ===== Série (logs) =====
#define SERIAL_BAUD     115200

// ===== I2C commun =====
#define I2C_FREQ_HZ     400000   // Teensy 4.1 OK
#define I2C_TIMEOUT_MS  20

// ===== IMU (BNO085 via STEMMA QT) =====
#define BNO_I2C_ADDR_HEX  0x4A   // 0x4B si DI tiré haut
// Axe choisi pour l’extension — on ajustera plus tard
#define EXT_AXIS_X       0.0f
#define EXT_AXIS_Y       1.0f
#define EXT_AXIS_Z       0.0f
#define EXT_SIGN         +1.0f   // mettre -1.0f si signe inversé

// ===== PCA9685 (servos) =====
#define PCA_I2C_ADDR_HEX  0x40
#define PCA_PWM_FREQ_HZ   50     // 50 Hz → 20 ms

// Canaux (adapter si besoin)
#define SERVO_CH_0  0
#define SERVO_CH_1  1
#define SERVO_CH_2  2

// Plages µs (à affiner selon tes servos)
#define SERVO0_US_MIN  800
#define SERVO0_US_NEU  1500
#define SERVO0_US_MAX  2200

#define SERVO1_US_MIN  800
#define SERVO1_US_NEU  1500
#define SERVO1_US_MAX  2200

#define SERVO2_US_MIN  800
#define SERVO2_US_NEU  1500
#define SERVO2_US_MAX  2200
