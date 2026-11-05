#include <Arduino.h>
#include <LibRobus.h>
#include <DetecteurSuiveurLigne.h>

namespace {
// Capteurs vus dans le sens de marche du robot.
const byte CAPTEUR_GAUCHE = A4;
const byte CAPTEUR_CENTRE = A5;
const byte CAPTEUR_DROITE = A6;

// Si la mesure atteint le seuil, le capteur voit la ligne noire.
const int SEUIL_GAUCHE = 105;
const int SEUIL_CENTRE = 146;
const int SEUIL_DROITE = 77;

const float VITESSE_AVANCE = 0.23f;
const float VITESSE_INTERIEURE = 0.12f;
const float VITESSE_EXTERIEURE = 0.27f;
enum Direction { TOUT_DROIT, GAUCHE, DROITE };
Direction dernierVirage = TOUT_DROIT;

void rouler(Direction direction, bool lentement) {
  float vitesseGauche = lentement ? 0.16f : VITESSE_AVANCE;
  float vitesseDroite = vitesseGauche;

  if (direction == GAUCHE) {
    // Pour tourner à gauche, la roue gauche tourne moins vite.
    vitesseGauche = lentement ? 0.08f : VITESSE_INTERIEURE;
    vitesseDroite = lentement ? 0.19f : VITESSE_EXTERIEURE;
  } else if (direction == DROITE) {
    // Pour tourner à droite, la roue droite tourne moins vite.
    vitesseGauche = lentement ? 0.19f : VITESSE_EXTERIEURE;
    vitesseDroite = lentement ? 0.08f : VITESSE_INTERIEURE;
  }

  MOTOR_SetSpeed(LEFT, vitesseGauche);
  MOTOR_SetSpeed(RIGHT, vitesseDroite);
}

} // namespace

void reinitialiserSuiveurLigne() {
  dernierVirage = TOUT_DROIT;
}

void initialiserSuiveurLigne() {
  pinMode(CAPTEUR_GAUCHE, INPUT);
  pinMode(CAPTEUR_CENTRE, INPUT);
  pinMode(CAPTEUR_DROITE, INPUT);
}

int detecterPositionRobot() {
  const bool gauche = analogRead(CAPTEUR_GAUCHE) >= SEUIL_GAUCHE;
  const bool centre = analogRead(CAPTEUR_CENTRE) >= SEUIL_CENTRE;
  const bool droite = analogRead(CAPTEUR_DROITE) >= SEUIL_DROITE;
  return (gauche << 2) | (centre << 1) | droite;
}

bool ligneCentree() {
  return detecterPositionRobot() == 2;
}

void suivreLigne() {
  const int position = detecterPositionRobot();
  const bool gauche = position & 4;
  const bool centre = position & 2;
  const bool droite = position & 1;
  int nombreActifs = int(gauche) + int(centre) + int(droite);

  if (nombreActifs == 1) {
    if (gauche) {
      dernierVirage = GAUCHE;
      rouler(GAUCHE, false);
    } else if (droite) {
      dernierVirage = DROITE;
      rouler(DROITE, false);
    } else {
      rouler(TOUT_DROIT, false);
    }
  } else {
    // Aucun capteur ou plusieurs capteurs actifs :
    // chercher lentement du côté du dernier virage.
    Direction recherche = dernierVirage;

    if (recherche == TOUT_DROIT) {
      recherche = GAUCHE;  // Aucun virage mémorisé : essayer à gauche.
    }

    rouler(recherche, true);
  }
}
