#ifndef PINCE_H
#define PINCE_H

#include <MegaServo.h>

//Déclaration variables globales
static const int pinServo = 4; // Pin du servo de la pince (à ajuster)
static const int angleOuverture = 0; // à ajuster
static const int angleFermeture = 90; // à ajuster

static MegaServo pince; // Objet servo pour la pince

//fonctions

void initialiserPince();

void pinceOuverture();

void pinceFermeture();

#endif