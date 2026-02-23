#pragma once
#include "Config.h"

float calculerPositionRetardee(float valeurActuelle, int delaiMs);
void preparerListeEssais();
void executerUnEssai(int numeroEssaiActuel);
bool experienceTerminee(int numeroEssaiActuel);