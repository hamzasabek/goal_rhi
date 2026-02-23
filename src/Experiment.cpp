#include "Experiment.h"
#include "Drivers.h"
#include "Sensors.h"
#include "Logging.h"

// ==========================================
// VARIABLES GLOBALES (Expérience)
// ==========================================
Essai listeEssais[10];              // Tableau contenant les 10 essais de l'expérience
float memoirePositions[tailleBuffer]; // Tableau pour stocker les positions passées (Buffer)
int indexEcriture = 0;              // Position actuelle où l'on écrit dans la mémoire

// Fonction pour enregistrer la position et récupérer la position retardée
float calculerPositionRetardee(float valeurActuelle, int delaiMs) {
  memoirePositions[indexEcriture] = valeurActuelle; // Enregistrer cet angle dans la mémoire (buffer)
  
  int casesAReculer = delaiMs / 20; // Calculer de combien de cases on doit reculer (1 case = 20ms)
  int indexLecture = (indexEcriture - casesAReculer + tailleBuffer) % tailleBuffer; // Calculer l'index du passé
  
  indexEcriture = (indexEcriture + 1) % tailleBuffer; // Avancer l'index d'écriture pour la prochaine fois
  
  return memoirePositions[indexLecture]; // Renvoyer l'angle retardé (0-90°)
}

void preparerListeEssais() {
  // Remplissage de la liste des essais
  int d[] = {0, 50, 150, 300, 500}; // Liste des délais possibles
  int index = 0; // Index pour remplir le tableau
  for (int i = 0; i < 5; i++) { // Pour chaque délai
    for (int j = 0; j < 2; j++) { // Pour chaque état de vibration (0 ou 1)
      listeEssais[index].delaiMs = d[i]; // Assigner le délai
      listeEssais[index].vibrationActive = (j == 1); // Assigner la vibration (Vrai si j=1)
      index++; // Passer à la case suivante
    }
  }

  // Mélange de la liste (Randomisation)
  randomSeed(analogRead(A1)); // Initialiser le hasard sur une broche vide
  for (int i = 0; i < 10; i++) { // Parcourir la liste
    int auHasard = random(0, 10); // Choisir une position au hasard
    Essai temporaire = listeEssais[i]; // Sauvegarder l'essai actuel
    listeEssais[i] = listeEssais[auHasard]; // Remplacer par l'essai au hasard
    listeEssais[auHasard] = temporaire; // Mettre l'essai actuel à la place de celui au hasard
  }
}

bool experienceTerminee(int numeroEssaiActuel) {
    return numeroEssaiActuel >= 10;
}

// ==========================================
// 6. EXÉCUTION D'UN ESSAI COMPLET
// ==========================================
void executerUnEssai(int numeroEssaiActuel) {
  Essai essaiEnCours = listeEssais[numeroEssaiActuel];
  
  Serial.print("\n========================================\n");
  Serial.print("=== PRÉPARATION DE L'ESSAI ");
  Serial.print(numeroEssaiActuel + 1);
  Serial.println(" ===");

  // 1. Mettre le servo (doigt en caoutchouc) en position initiale (0°)
  bougerMainCaoutchouc(0);
  delay(300); // Laisse le temps au servo de se remettre en place
  
  // 2. Calibrer le capteur du participant sur son nouveau "zéro"
  calibrerCapteur();

  // 3. Attendre le signal de départ de l'expérimentateur
  Serial.println("\n>>> TAPEZ 'd' ET FAITES ENTRÉE POUR DÉMARRER L'ESSAI <<<");
  while (true) {
    if (Serial.available() > 0) {
      char touche = Serial.read();
      if (touche == 'd' || touche == 'D') {
        Serial.println(">>> DÉMARRAGE DE L'ESSAI ! <<<");
        break; // On sort de la boucle infinie d'attente
      }
    }
    delay(10); // Petite pause pour ne pas surcharger la boucle d'attente
  }

  // --- LE RESTE DU CODE RESTE INCHANGÉ ---
  unsigned long chronoDebut = millis(); 
  unsigned long dernierTempsBoucle = millis(); 

  reinitialiserMouvements(); 
  gererVibrations(essaiEnCours.vibrationActive); 

  while (getMouvementsEffectues() < nombreMouvementsRequis && (millis() - chronoDebut) < tempsMaximumEssai) {

    if (millis() - dernierTempsBoucle >= 20) {
      dernierTempsBoucle = millis(); 

      float lectureAngle = lireAngleDoigt(); 
      
      mettreAJourCompteurMouvement(lectureAngle); 
      
      float angleRetarde = calculerPositionRetardee(lectureAngle, essaiEnCours.delaiMs); 
      bougerMainCaoutchouc(angleRetarde); 
    }
  }

  gererVibrations(false); 
  unsigned long tempsFinal = millis() - chronoDebut; 
  
  int reponseParticipant = obtenirReponse(); 
  
  envoyerResultats(essaiEnCours, numeroEssaiActuel, getMouvementsEffectues(), tempsFinal, reponseParticipant); 
  
  delay(1000); 
}