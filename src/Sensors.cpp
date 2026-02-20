#include "Sensors.h"

// ==========================================
// VARIABLES GLOBALES (Capteurs)
// ==========================================
int mouvementsEffectues = 0;      // Compteur de mouvements pour l'essai en cours
bool doigtEstEnHaut = false;      // Variable pour savoir si le doigt vient de monter

void initSensors() {
  pinMode(pinBoutonOui, INPUT_PULLUP); // Configurer bouton Oui avec résistance interne
  pinMode(pinBoutonNon, INPUT_PULLUP); // Configurer bouton Non avec résistance interne
}

int lirePotentiometre() {
  return analogRead(pinPotentiometre); // Lire la position du doigt
}

void reinitialiserMouvements() {
  mouvementsEffectues = 0; // Remettre le compteur de mouvements à zéro
  doigtEstEnHaut = false;  // Réinitialiser l'état du doigt
}

int getMouvementsEffectues() {
  return mouvementsEffectues;
}

// Fonction pour compter les cycles de mouvement (Hystérésis)
void mettreAJourCompteurMouvement(int valeurPot) {
  if (doigtEstEnHaut == false && valeurPot > seuilHaut) { // Si on était en bas et qu'on dépasse le seuil haut
    doigtEstEnHaut = true; // On marque que le doigt est maintenant en haut
  } 
  else if (doigtEstEnHaut == true && valeurPot < seuilBas) { // Si on était en haut et qu'on redescend sous le seuil bas
    doigtEstEnHaut = false; // On marque que le doigt est redescendu
    mouvementsEffectues++; // On compte cela comme un mouvement complet réalisé
    Serial.print("Mouvement valide : "); // Afficher un message de suivi
    Serial.println(mouvementsEffectues); // Afficher le nombre actuel
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