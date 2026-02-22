#include "Drivers.h"
#include <Wire.h> // Nécessaire pour la communication I2C
#include <Adafruit_PWMServoDriver.h> // Bibliothèque pour le PCA9685

Adafruit_PWMServoDriver pca = Adafruit_PWMServoDriver(); // Création de l'objet PCA

void initDrivers() {
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
  // On garde la conversion pour le PCA9685
  // Rappel : l'angle arrive ici déjà traité (entre 0 et 180)
  int pulse = map((int)angle, 0, 180, 150, 600);
  pca.setPWM(canalServoPCA, 0, pulse); 
}