#ifndef DETERCTEUR_PROXIMITE_H
#define DETERCTEUR_PROXIMITE_H

#include <Arduino.h>

enum EtatRobot {
  ETAT_ARRET = 0,
  ETAT_AVANCE = 1,
  ETAT_RECULE = 2,
  ETAT_TOURNE_DROIT = 3,
  ETAT_TOURNE_GAUCHE = 4
};

extern const int vertpin;
extern const int rougepin;
extern bool vert;
extern bool rouge;

void initialiserDetecteurProximite();
void mettreAJourEtatAvecDetecteurs();

#endif
