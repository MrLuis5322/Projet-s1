#ifndef TRAITEMENT_COULEUR_H
#define TRAITEMENT_COULEUR_H


// Définition des couleurs
enum CouleurId {
    AUCUN,
    ROSE,
    JAUNE,
    ORANGE,
    BLEU,
    VERT
};

// Structure RGB
struct Couleur {
    float R;
    float G;
    float B;
};

// Constante
const int NB_ECHANTILLONS = 10;

// Fonctions
bool initialiserCapteurCouleur();

void ajouterEchantillon(Couleur c);

void viderEchantillons();

Couleur moyenneCouleur();

CouleurId identifierCouleur(Couleur m);

String nomCouleur(CouleurId id);

Couleur lireCapteur();

void calibrerCouleur(const char* nom, int duree_ms);

void modeCalibration();


#endif