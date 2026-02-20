#pragma once
#include "Config.h"

void initSensors();
int lirePotentiometre();
void reinitialiserMouvements();
void mettreAJourCompteurMouvement(int valeurPot);
int getMouvementsEffectues();
int obtenirReponse();