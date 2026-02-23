#include "Sensors.h"
#include <Adafruit_BNO08x.h>
// ==========================================
// VARIABLES GLOBALES (Capteurs)
// ==========================================
int mouvementsEffectues = 0;      // Compteur de mouvements pour l'essai en cours
bool doigtEstEnHaut = false;      // Variable pour savoir si le doigt vient de monter
Adafruit_BNO08x bno08x; // L'objet qui représente notre capteur IMU

void initSensors() {
  // 1. ON INITIALISE LES BOUTONS QUEL QUE SOIT LE CAPTEUR !
  pinMode(pinBoutonOui, INPUT_PULLUP);
  pinMode(pinBoutonNon, INPUT_PULLUP);

  // 2. Initialisation du capteur choisi
  if (CAPTEUR_ACTUEL == IMU_BNO085) {
    Wire.begin();
    Wire.setClock(100000); 

    if (!bno08x.begin_I2C()) {
      Serial.println("Erreur : BNO085 non détecté !");
      while (1); 
    } else {
      Serial.println("Capteur BNO085 détecté ! 🛰️");
      bno08x.enableReport(SH2_GAME_ROTATION_VECTOR, 20000); 
    }
  }
}

float lireAngleDoigt() {
    if (CAPTEUR_ACTUEL == IMU_BNO085) {
        float angleBrut = lireIMU(); // Ça donne environ de -45° à +45°
        
        // On convertit les angles de l'IMU vers l'échelle 0-90° pour le servo
        float angleConverti = map(angleBrut, -45, 45, 0, 90);
        
        // On s'assure que ça ne dépasse jamais les limites physiques du servo
        return constrain(angleConverti, 0, 90); 
        
    } else {
        int brute = lirePotentiometre(); // 0-1023
        return map(brute, 0, 1023, 0, 90); 
    }
}


float lireIMU() {
  if (bno08x.wasReset()) {
    bno08x.enableReport(SH2_GAME_ROTATION_VECTOR, 20000);
  }

  sh2_SensorValue_t sensorValue;
  static float axeLisse = 0.0;

  while (bno08x.getSensorEvent(&sensorValue)) {
    float qr = sensorValue.un.gameRotationVector.real;
    float qi = sensorValue.un.gameRotationVector.i;
    float qj = sensorValue.un.gameRotationVector.j;
    float qk = sensorValue.un.gameRotationVector.k;

    // 1. Calcul du Roll (Roulis)
    float roll = atan2(2.0 * (qr * qi + qj * qk), 1.0 - 2.0 * (qi * qi + qj * qj)) * 180.0 / PI;
    
    // 2. Calcul du Pitch (Tangage) - avec protection mathématique pour le asin()
    float sinp = 2.0 * (qr * qj - qk * qi);
    sinp = constrain(sinp, -1.0, 1.0); // Évite les erreurs NaN
    float pitch = asin(sinp) * 180.0 / PI;
    
    // 3. Calcul du Yaw (Lacet)
    float yaw = atan2(2.0 * (qr * qk + qi * qj), 1.0 - 2.0 * (qj * qj + qk * qk)) * 180.0 / PI;

    // --- CHOISIS TON AXE ICI ---
    // Remplace "pitch" par "roll" ou "yaw" si le capteur est monté de travers !
    float axeChoisi = pitch; 

    // Lissage nerveux (80% direct, 20% inertie)
    if (axeLisse == 0.0) axeLisse = axeChoisi; 
    axeLisse = (axeLisse * 0.2) + (axeChoisi * 0.8);
  }
  
  return axeLisse; 
}



int lirePotentiometre() {
  // Lit la valeur de la broche (qui donne un chiffre entre 0 et 1023)
  return analogRead(pinPotentiometre); 
}

void reinitialiserMouvements() {
  mouvementsEffectues = 0; // Remettre le compteur de mouvements à zéro
  doigtEstEnHaut = false;  // Réinitialiser l'état du doigt
}

int getMouvementsEffectues() {
  return mouvementsEffectues;
}

// Fonction pour compter les cycles de mouvement (Hystérésis)
void mettreAJourCompteurMouvement(float angle) {
  float seuilHautActif;
  float seuilBasActif;

  // 1. Choix des seuils selon le capteur utilisé
  if (CAPTEUR_ACTUEL == IMU_BNO085) {
    seuilHautActif = IMU_SEUIL_HAUT;
    seuilBasActif = IMU_SEUIL_BAS;
  } else {
    seuilHautActif = POT_SEUIL_HAUT;
    seuilBasActif = POT_SEUIL_BAS;
  }

  // 2. Vérification des mouvements avec les bons seuils
  if (doigtEstEnHaut == false && angle > seuilHautActif) { 
    doigtEstEnHaut = true; // On marque que le doigt est maintenant en haut
  } 
  else if (doigtEstEnHaut == true && angle < seuilBasActif) { 
    doigtEstEnHaut = false; // On marque que le doigt est redescendu
    mouvementsEffectues++; // On compte cela comme un mouvement complet
    Serial.print("Mouvement valide : "); 
    Serial.println(mouvementsEffectues); 
  }
}

// Fonction qui attend que le participant appuie sur un bouton OU utilise le Moniteur Série
int obtenirReponse() {
  Serial.println("EN ATTENTE DE REPONSE : Appuyez sur Boutons OU tapez 'o' (Oui) / 'n' (Non) dans le moniteur...");
  
  while (true) { 
    // 1. Vérification des boutons physiques (votre code original)
    if (digitalRead(pinBoutonOui) == LOW) return 1; 
    if (digitalRead(pinBoutonNon) == LOW) return 0; 
    
    // 2. Vérification du Moniteur Série (Ajout pour le clavier)
    if (Serial.available() > 0) {
      char touche = Serial.read(); // Lire le caractère tapé
      if (touche == 'o' || touche == 'O') return 1; // 'o' pour OUI
      if (touche == 'n' || touche == 'N') return 0; // 'n' pour NON
    }
  }
}