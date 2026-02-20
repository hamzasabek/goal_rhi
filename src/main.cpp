#include <Arduino.h>
#include "Config.h"
#include "Drivers.h"
#include "Sensors.h"
#include "Logging.h"
#include "Experiment.h"

int numeroEssaiActuel = 0; // Compteur pour savoir quel essai on est en train de faire

// ==========================================
// 7. SETUP ET LOOP PRINCIPALE
// ==========================================
void setup() {
  Serial.begin(115200); // Démarrer la communication série rapide
  
  initDrivers();        // Initialise le PCA9685 et les vibreurs
  initSensors();        // Initialise les boutons
  preparerListeEssais();// Crée et mélange la liste des 10 essais
  initLogging();        // Affiche l'en-tête CSV
}

void loop() {
  if (!experienceTerminee(numeroEssaiActuel)) { // Si on n'a pas encore fini les 10 essais
    executerUnEssai(numeroEssaiActuel); // Faire l'essai
    numeroEssaiActuel++; // Passer au numéro suivant
  } else { // Si c'est fini
    Serial.println("EXPÉRIENCE TERMINÉE"); // Message de fin
    while(1); // Bloquer le programme ici
  }
}