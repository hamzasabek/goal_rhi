#include "Logging.h"

void initLogging() {
  Serial.println("PRÊT : Trial,Delay,Noise,Moves,Time,Response"); // En-tête du fichier de données
}

// Fonction pour envoyer les résultats au format CSV vers l'ordinateur
void envoyerResultats(Essai e, int numEssai, int nbMouv, unsigned long temps, int rep) {
  Serial.print("DATA,"); // Tag pour identifier les données
  Serial.print(numEssai + 1); Serial.print(","); // Numéro de l'essai
  Serial.print(e.delaiMs); Serial.print(","); // Le délai testé
  Serial.print(e.vibrationActive); Serial.print(","); // Si vibration il y avait
  Serial.print(nbMouv); Serial.print(","); // Nombre de mouvements faits
  Serial.print(temps / 1000.0); Serial.print(","); // Temps total en secondes
  Serial.println(rep == 1 ? "OUI" : "NON"); // Réponse finale du participant
}