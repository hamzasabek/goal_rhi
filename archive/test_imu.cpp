#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>

// NOUVEAUX NOMS pour ne pas fâcher Sensors.cpp !
Adafruit_BNO08x bnoTest;
sh2_SensorValue_t sensorValueTest;

void setup() {
  Serial.begin(115200);
  
  // On laisse 3 secondes au Teensy pour stabiliser l'USB
  delay(3000); 
  Serial.println("\n--- DEMARRAGE DU TEST BNO085 SECURISE ---");

  // 1. On démarre manuellement l'I2C à une vitesse stable (100kHz)
  Wire.begin();
  Wire.setClock(100000); 
  Serial.println("Bus I2C demarre. Recherche du capteur...");

  // 2. On tente la connexion au capteur
  if (!bnoTest.begin_I2C()) {
    Serial.println("ERREUR : Capteur BNO085 introuvable !");
    Serial.println("Mais le Teensy n'a pas plante, l'USB fonctionne !");
    while (1) {
      delay(100); // Boucle infinie sécurisée qui ne crashe pas l'USB
    }
  }

  Serial.println("SUCCES : BNO085 detecte et pret !");
  bnoTest.enableReport(SH2_GAME_ROTATION_VECTOR, 50000); 
}

void loop() {
  // On maintient l'USB en vie
  delay(10); 

  if (bnoTest.getSensorEvent(&sensorValueTest)) {
    if (sensorValueTest.sensorId == SH2_GAME_ROTATION_VECTOR) {
      float qr = sensorValueTest.un.gameRotationVector.real;
      float qi = sensorValueTest.un.gameRotationVector.i;
      float qj = sensorValueTest.un.gameRotationVector.j;
      float qk = sensorValueTest.un.gameRotationVector.k;

      float sqr = sq(qr); float sqi = sq(qi); float sqj = sq(qj); float sqk = sq(qk);
      float pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr)) * 180.0 / PI;
      float roll = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr)) * 180.0 / PI;
      float yaw = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr)) * 180.0 / PI;

      Serial.print("PITCH: "); Serial.print(pitch);
      Serial.print(" | ROLL: "); Serial.print(roll);
      Serial.print(" | YAW: "); Serial.println(yaw);
    }
  }
}