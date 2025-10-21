#pragma once
#include <Arduino.h>
#include "config.h"

// --- Représentation d'orientation : quaternion (on s'en servira vraiment plus tard)
struct Quat {
  float w = 1.f, x = 0.f, y = 0.f, z = 0.f;
};

// --- Driver IMU (squelette sans accès matériel)
class ImuDriver {
public:
  // Appelé au démarrage: ici on ne touche pas au capteur (pas branché)
  bool begin() {
    // plus tard: init I2C + activer report(s)
    _ready = true;
    return true;
  }

  // Mise à jour des mesures (ici: on simule juste qu'on a des données)
  void update() {
    // plus tard: lire un event BNO085 → remplir _q
    // pour l'instant on laisse la valeur par défaut
  }

  // Capturer le "neutre" (on met la valeur actuelle comme référence)
  void captureNeutral() {
    _q0 = _q;
    _haveNeutral = true;
  }

  // Définir l'axe d'extension à partir de la config (X/Y/Z + signe)
  void setAxisFromConfig() {
    _ax = EXT_AXIS_X;
    _ay = EXT_AXIS_Y;
    _az = EXT_AXIS_Z;
    _sign = EXT_SIGN;
  }

  // Angle "extension" en degrés — pour l'instant 0 (pas de calcul hardware)
  float angleDeg() const {
    // plus tard: Δq = q0^{-1} ⊗ q → angle signé autour de (_ax,_ay,_az) * _sign
    return 0.0f;
  }

  // Accès lecture (ex. debug)
  Quat lastQuat() const { return _q; }
  bool ready() const { return _ready; }
  bool haveNeutral() const { return _haveNeutral; }

private:
  bool _ready = false;
  bool _haveNeutral = false;
  Quat _q0;     // neutre
  Quat _q;      // dernière lecture (plus tard)
  float _ax = 0.f, _ay = 1.f, _az = 0.f; // axe par défaut = Y
  float _sign = +1.f;
};
