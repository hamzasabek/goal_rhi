#include "Experiment.h"
#include "Drivers.h"
#include "Sensors.h"
#include "Logging.h"

// ==========================================
// VARIABLES GLOBALES (Expérience)
// ==========================================
Essai listeEssais[10];              // Tableau contenant les 10 essais de l'expérience
int memoirePositions[tailleBuffer]; // Tableau pour stocker les positions passées (Buffer)
int indexEcriture = 0;              // Position actuelle où l'on écrit dans la mémoire

// Fonction pour enregistrer la position et récupérer la position retardée
int calculerPositionRetardee(int valeurActuelle, int delaiMs) {
  int angle = map(valeurActuelle, 0, 1023, 0, 180); // Transformer la valeur 0-1023 en angle 0-180
  memoirePositions[indexEcriture] = angle; // Enregistrer cet angle dans la mémoire (buffer)
  
  int casesAReculer = delaiMs / 20; // Calculer de combien de cases on doit reculer (1 case = 20ms)
  int indexLecture = (indexEcriture - casesAReculer + tailleBuffer) % tailleBuffer; // Calculer l'index du passé
  
  indexEcriture = (indexEcriture + 1) % tailleBuffer; // Avancer l'index d'écriture pour la prochaine fois
  
  return memoirePositions[indexLecture]; // Renvoyer la vieille position enregistrée
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
  unsigned long chronoDebut = millis(); // Noter l'heure de début
  
  reinitialiserMouvements(); // Remettre le compteur de mouvements à zéro et réinitialiser l'état du doigt
  gererVibrations(essaiEnCours.vibrationActive); // Allumer les vibreurs si nécessaire

  // Boucle de mouvement : on continue tant que les deux conditions ne sont pas atteintes
  while (getMouvementsEffectues() < nombreMouvementsRequis && (millis() - chronoDebut) < tempsMaximumEssai) {
    int lecturePot = lirePotentiometre(); // Lire la position du doigt
    
    mettreAJourCompteurMouvement(lecturePot); // Vérifier si un mouvement est fait
    
    int angleRetarde = calculerPositionRetardee(lecturePot, essaiEnCours.delaiMs); // Calculer le retard
    bougerMainCaoutchouc(angleRetarde); // Faire bouger la main via le PCA9685
    
    delay(20); // Attendre 20ms pour avoir une fréquence stable de 50Hz
  }

  gererVibrations(false); // Éteindre les vibreurs à la fin du mouvement
  unsigned long tempsFinal = millis() - chronoDebut; // Calculer le temps total écoulé
  
  int reponseParticipant = obtenirReponse(); // Demander si la main était la sienne
  
  envoyerResultats(essaiEnCours, numeroEssaiActuel, getMouvementsEffectues(), tempsFinal, reponseParticipant); // Enregistrer
  
  delay(1000); // Petite pause avant l'essai suivant
}