#ifndef DETECTEUR_PROXIMITE_H
#define DETECTEUR_PROXIMITE_H

#include <Arduino.h>


extern const int vertpin;
extern const int rougepin;
extern bool vert;
extern bool rouge;
extern bool ObstacleDetecte;

void initialiserDetecteurProximite();
void detecterObstacle();

#endif
