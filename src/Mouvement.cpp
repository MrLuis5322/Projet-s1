/*
Mouvement - P13
Alexandre H.
Code pour faire bouger et tourner le robot
01/10/2026
*/

#include <LibRobus.h>
#include <Mouvement.h>
#include <DetecteurProximite.h>

const float DiametreRoue = 7.8; // Toutes les distances sont en centimetres.
const float TICKS_PAR_TOUR_G = 3200;
const float TICKS_PAR_TOUR_D = 3220;
const float CIRCONFERENCE_DE_LA_ROUE = 3.14159 * DiametreRoue;
const float CM_PAR_TICK_G = CIRCONFERENCE_DE_LA_ROUE / TICKS_PAR_TOUR_G;
const float CM_PAR_TICK_D = CIRCONFERENCE_DE_LA_ROUE / TICKS_PAR_TOUR_D;
const float ECART_ROUES = 17.5;
const float CIRCONFERENCE_TOURNER = 3.14159 * ECART_ROUES;

float facteur_vitesse = 1.5;
float vitesse_Gauche = 0.40 * facteur_vitesse;
float vitesse_Droite = 0.415 * facteur_vitesse;

float facteur_vitesse_tourne = 1;
float vitesse_Gauche_Tourne = 0.40 * facteur_vitesse_tourne;
float vitesse_Droite_Tourne = 0.408 * facteur_vitesse_tourne;

// Arrete immediatement les deux moteurs.
void arret() {
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
}

// Convertit une distance en centimetres en ticks pour l'encodeur choisi.
static long distanceEnTicks(float distanceCM, int encodeur) {
  if (encodeur == 0) {
    return lround(distanceCM / CM_PAR_TICK_G);
  }
  return lround(distanceCM / CM_PAR_TICK_D);
}

// Reculer du nombre de ticks mesure pendant l'avance permet a une sonde
// interrompue par un mur de repartir approximativement du centre de sa case.
static void revenirAuPointDeDepart() {
  arret();
  delay(100);

  long encodeurG = ENCODER_Read(0);
  long encodeurD = ENCODER_Read(1);
  MOTOR_SetSpeed(LEFT, encodeurG > 10 ? -vitesse_Gauche * 0.35f : 0);
  MOTOR_SetSpeed(RIGHT, encodeurD > 10 ? -vitesse_Droite * 0.35f : 0);

  // Chaque moteur s'arrete lorsqu'il a annule sa propre avance.
  while (ENCODER_Read(0) > 10 || ENCODER_Read(1) > 10) {
    if (ENCODER_Read(0) <= 10) {
      MOTOR_SetSpeed(LEFT, 0);
    }
    if (ENCODER_Read(1) <= 10) {
      MOTOR_SetSpeed(RIGHT, 0);
    }
    delay(1);
  }

  arret();
}

// Avance de la distance demandee. Retourne false si le capteur avant detecte
// un obstacle; dans ce cas, le robot est ramene vers son point de depart.
bool mouvementAvant(float distanceCM) {
  if (distanceCM <= 0) {
    arret();
    return true;
  }

  long CibleG = distanceEnTicks(distanceCM, 0);
  long CibleD = distanceEnTicks(distanceCM, 1);

  ENCODER_Reset(0);
  ENCODER_Reset(1);
  ObstacleDetecte = false;

  MOTOR_SetSpeed(LEFT, vitesse_Gauche);
  MOTOR_SetSpeed(RIGHT, vitesse_Droite);

  bool ralentissement1 = false;
  bool ralentissement2 = false;

  // Le OR garantit que les deux roues atteignent leur cible, meme si une roue
  // prend un peu d'avance sur l'autre.
  while (ENCODER_Read(0) < CibleG || ENCODER_Read(1) < CibleD) {
    long encodeurG = ENCODER_Read(0);
    long encodeurD = ENCODER_Read(1);

    detecterObstacle();
    if (ObstacleDetecte) {
      revenirAuPointDeDepart();
      ObstacleDetecte = false;
      return false;
    }

    // Deux ralentissements limitent le depassement a l'approche de la cible.
    if (!ralentissement1 &&
        encodeurG >= CibleG - 700 &&
        encodeurD >= CibleD - 700) {
      MOTOR_SetSpeed(LEFT, vitesse_Gauche * 0.7f);
      MOTOR_SetSpeed(RIGHT, vitesse_Droite * 0.7f);
      ralentissement1 = true;
    }

    if (!ralentissement2 &&
        encodeurG >= CibleG - 400 &&
        encodeurD >= CibleD - 400) {
      MOTOR_SetSpeed(LEFT, vitesse_Gauche * 0.35f);
      MOTOR_SetSpeed(RIGHT, vitesse_Droite * 0.35f);
      ralentissement2 = true;
    }

    delay(1);
  }

  arret();
  return true;
}

// Fait pivoter le robot autour de son centre; un angle positif tourne a droite.
void tourne(float angleDegres) {
  if (angleDegres == 0) {
    arret();
    return;
  }

  float distanceRoue =
      (abs(angleDegres) * CIRCONFERENCE_TOURNER) / 360.0f;
  long CibleG = distanceEnTicks(distanceRoue, 0);
  long CibleD = distanceEnTicks(distanceRoue, 1);

  ENCODER_Reset(0);
  ENCODER_Reset(1);

  if (angleDegres > 0) {
    MOTOR_SetSpeed(LEFT, vitesse_Gauche_Tourne);
    MOTOR_SetSpeed(RIGHT, -vitesse_Droite_Tourne);
  } else {
    MOTOR_SetSpeed(LEFT, -vitesse_Gauche_Tourne);
    MOTOR_SetSpeed(RIGHT, vitesse_Droite_Tourne);
  }

  // Les deux encodeurs doivent atteindre leur cible pour terminer la rotation.
  while (abs(ENCODER_Read(0)) < CibleG ||
         abs(ENCODER_Read(1)) < CibleD) {
    delay(1);
  }

  arret();
}

// Annule une avance interrompue et retrouve le point de depart a l'aide des
// ticks encore disponibles dans les encodeurs.
void annulerMouvement() {
  revenirAuPointDeDepart();
  ObstacleDetecte = false;
}
