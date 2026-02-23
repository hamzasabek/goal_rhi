#pragma once
#include "Config.h"

void initSensors();
float lireAngleDoigt();
float lireIMU();
int lirePotentiometre();
void reinitialiserMouvements();
void mettreAJourCompteurMouvement(float angle);
int getMouvementsEffectues();
int obtenirReponse();
void calibrerCapteur();