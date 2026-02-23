#include "Drivers.h"
#include <Wire.h> // Nécessaire pour la communication I2C
#include <Adafruit_PWMServoDriver.h> // Bibliothèque pour le PCA9685

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(0x40, Wire2); // Création de l'objet PCA

void initDrivers() {
  Wire2.begin();
  Wire2.setClock(100000);
  pca.begin();          // Initialiser le driver PCA9685
  pca.setPWMFreq(50);   // Servos standards fonctionnent à 50Hz
  
  pinMode(pinVibreur1, OUTPUT); // Configurer vibreur 1 en sortie
  pinMode(pinVibreur2, OUTPUT); // Configurer vibreur 2 en sortie
}

// Fonction pour allumer ou éteindre les vibreurs
void gererVibrations(bool etat) {
  if (etat == true) {               // Si l'essai demande du bruit (vibration)
    analogWrite(pinVibreur1, 180);  // Allumer vibreur 1 à puissance moyenne
    analogWrite(pinVibreur2, 180);  // Allumer vibreur 2 à puissance moyenne
  } else {                          // Sinon
    digitalWrite(pinVibreur1, LOW); // Éteindre vibreur 1
    digitalWrite(pinVibreur2, LOW); // Éteindre vibreur 2
  }
}

void bougerMainCaoutchouc(float angle) {
  // 1. INVERSION : On transforme l'angle pour le moteur
  // Si ton doigt est à 0 (repos), le moteur reçoit 90 (haut pour lui).
  // Si ton doigt est à 90 (levé), le moteur reçoit 0 (bas pour lui).
  float anglePourMoteur = 90.0 - angle;

  // 2. CONVERSION : On transforme cet angle en signal PWM (Pulse)
  // On garde ici le map standard 0->150 et 90->600
  int pulse = map((int)anglePourMoteur, 0, 90, 150, 600);
  
  // 3. SÉCURITÉ : On ne dépasse pas les bornes du PCA9685
  pulse = constrain(pulse, 150, 600); 
  
  // 4. ENVOI :
  pca.setPWM(canalServoPCA, 0, pulse); 
}