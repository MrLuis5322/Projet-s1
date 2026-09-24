#include <DetercteurProximite.h>
#include <Mouvement.h>

// Pins des deux capteurs et dernieres valeurs lues.
const int vertpin = 48;
const int rougepin = 49;
bool vert = false;
bool rouge = false;

// Configure les pins en entree. Cette fonction est appelee une fois dans setup().
void initialiserDetecteurProximite() {
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
}

// Lit les capteurs et traduit leur combinaison en mouvement.
// Les constantes ETAT_* sont definies dans DetercteurProximite.h.
void mettreAJourEtatAvecDetecteurs() {
  vert = digitalRead(vertpin);
  rouge = digitalRead(rougepin);

  // Un robot a l'arret reste arrete jusqu'a ce qu'une autre logique le demarre.
  if (etat > ETAT_ARRET) {
    // Les deux capteurs voient la voie libre: avancer.
    if (vert && rouge) {
      etat = ETAT_AVANCE;
    // Les deux capteurs detectent un obstacle: reculer.
    } else if (!vert && !rouge) {
      etat = ETAT_RECULE;
    // Obstacle a gauche: tourner a droite.
    } else if (!vert && rouge) {
      etat = ETAT_TOURNE_DROIT;
    // Obstacle a droite: tourner a gauche.
    } else {
      etat = ETAT_TOURNE_GAUCHE;
    }
  }
}
