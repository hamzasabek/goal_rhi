#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include <Adafruit_PWMServoDriver.h>

// --- CONFIGURATION ---
Adafruit_BNO08x bnoTest;
sh2_SensorValue_t sensorValueTest;

// LA MAGIE EST ICI : On passe sur Wire2 pour les broches 24 et 25 !
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40, Wire2);

#define SERVO_NUM 0 
#define MIN_PULSE 150 
#define MAX_PULSE 600 

void setup() {
  Serial.begin(115200);
  delay(3000); 
  Serial.println("\n--- BNO085 (Wire) ET PCA9685 (Wire2) ---");

  // 1. Initialisation Bus 0 (Pour le BNO085 sur pins 18/19)
  Wire.begin();
  Wire.setClock(100000); 

  // 2. Initialisation Bus 2 (Pour le PCA9685 sur pins 24/25)
  Wire2.begin();
  Wire2.setClock(100000); 

  Serial.println("Initialisation du BNO085...");
  if (!bnoTest.begin_I2C()) {
    Serial.println("ERREUR BNO085");
    while(1);
  }
  bnoTest.enableReport(SH2_GAME_ROTATION_VECTOR, 50000); 
  Serial.println("BNO085 OK.");

  Serial.println("Initialisation du PCA9685...");
  pwm.begin();
  pwm.setPWMFreq(60); 
  Serial.println("PCA9685 OK.");
}

void loop() {
  // Si le BNO fait un petit caprice (les fameux "rarement"), on le relance en silence
  if (bnoTest.wasReset()) {
    bnoTest.enableReport(SH2_GAME_ROTATION_VECTOR, 50000);
  }

  while (bnoTest.getSensorEvent(&sensorValueTest)) {
    if (sensorValueTest.sensorId == SH2_GAME_ROTATION_VECTOR) {
      
      float qr = sensorValueTest.un.gameRotationVector.real;
      float qi = sensorValueTest.un.gameRotationVector.i;
      float qj = sensorValueTest.un.gameRotationVector.j;
      float qk = sensorValueTest.un.gameRotationVector.k;

      float sqr = sq(qr); float sqi = sq(qi); float sqj = sq(qj); float sqk = sq(qk);
      float pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr)) * 180.0 / PI;

    // Lissage nerveux pour un mouvement instantané
      static float pitchLisse = 0;
      if (pitchLisse == 0) pitchLisse = pitch; // Pour éviter un saut brutal au démarrage
      pitchLisse = (pitchLisse * 0.2) + (pitch * 0.8);

      int servoPulse = map(pitchLisse, -45, 45, MIN_PULSE, MAX_PULSE);
      servoPulse = constrain(servoPulse, MIN_PULSE, MAX_PULSE);

      // Envoi au servo (qui écoute maintenant sur Wire2)
      pwm.setPWM(SERVO_NUM, 0, servoPulse);

      Serial.print("PITCH: "); Serial.println(pitchLisse, 1);
    }
  }
  delay(1);
}