#ifndef TRAITEMENT_COULEUR_H
#define TRAITEMENT_COULEUR_H

#include <Arduino.h>

// ---------------------------------------------------------------
// DÉFINITION DES COULEURS
// ---------------------------------------------------------------
enum CouleurId {AUCUN, ROSE, JAUNE, ORANGE, BLEU, VERT, PLANCHER, LIGNE};

struct Couleur {
    float R;
    float G;
    float B;
};

// ---------------------------------------------------------------
// CONSTANTE
// ---------------------------------------------------------------
// Nombre d'échantillons pour trouver la couleur du parcours
const int NB_ECHANTILLONS = 10;

// ---------------------------------------------------------------
// FONCTIONS 
// ---------------------------------------------------------------

void ajouterEchantillon(Couleur c);

void viderEchantillons();

Couleur moyenneCouleur();

CouleurId identifierCouleur(Couleur m);

String nomCouleur(CouleurId id);

Couleur lireCapteur();

void calibrerCouleur(const char* nom, int duree_ms);

#endif