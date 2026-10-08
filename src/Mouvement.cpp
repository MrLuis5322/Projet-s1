/*
Mouvement - P13
Alexandre H.
Code pour faire bouger et tourner le robot
01/10/2026
*/

#include <LibRobus.h>
#include <Mouvement.h>
#include <DetecteurProximite.h>


const float DiametreRoue = 7.8; // TOUT est en CM
const float TICKS_PAR_TOUR_G = 3200; 
const float TICKS_PAR_TOUR_D = 3220; 
const float CIRCONFERENCE_DE_LA_ROUE = 3.14159*DiametreRoue;
const float CM_PAR_TICK_G = CIRCONFERENCE_DE_LA_ROUE/TICKS_PAR_TOUR_G;
const float CM_PAR_TICK_D = CIRCONFERENCE_DE_LA_ROUE/TICKS_PAR_TOUR_D;
const float ECART_ROUES = 17.5;
const float DEGREES_PAR_CM = 360/ECART_ROUES;
const float CIRCONFERENCE_TOURNER = 3.14159*ECART_ROUES;  

float facteur_vitesse = 1.5;

float vitesse_Gauche = 0.40*facteur_vitesse;
float vitesse_Droite = 0.415*facteur_vitesse;

float facteur_vitesse_tourne = 1;
float vitesse_Gauche_Tourne = 0.40*facteur_vitesse_tourne;
float vitesse_Droite_Tourne = 0.408*facteur_vitesse_tourne;

// -----------------------------------------------------------------------------
// Cette fonction arrête immédiatement les deux moteurs pour finir un déplacement
// ou interrompre un mouvement de sécurité.
// -----------------------------------------------------------------------------
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
}

// -----------------------------------------------------------------------------
// Convertit une distance en centimètres en ticks d'encodeur.
// Le paramètre encodeur permet de choisir entre le gauche et le droit, car les
// roues n'ont pas exactement le même nombre de ticks par tour.
// -----------------------------------------------------------------------------
long distanceEnTicks(float distanceCM, int encodeur) {
  if (encodeur == 0) {
    return lround(distanceCM / CM_PAR_TICK_G);
  }
  return lround(distanceCM / CM_PAR_TICK_D);
}


// -----------------------------------------------------------------------------
// Avance le robot d'une distance donnée en centimètres.
// La boucle suit la cible d'encodeur avec deux phases de ralentissement pour
// arriver au point final plus proprement et réduire les écarts de position.
// -----------------------------------------------------------------------------
void mouvementAvant(float distanceCM) {
  long CibleG = distanceEnTicks(distanceCM, 0);
  long CibleD = distanceEnTicks(distanceCM, 1);

  ENCODER_Reset(0);
  ENCODER_Reset(1);
  
  MOTOR_SetSpeed(LEFT, vitesse_Gauche);
  MOTOR_SetSpeed(RIGHT, vitesse_Droite);


  bool ralentissement1 = false;
  bool ralentissement2 = false;

  // On avance tant que les deux roues n'ont pas atteint leur cible respective.
  while (ENCODER_Read(0) < CibleG && ENCODER_Read(1) < CibleD) {
    
    long encodeurG = ENCODER_Read(0);
    long encodeurD = ENCODER_Read(1);

    // Diagnostic utile pendant le développement pour vérifier la progression.
    Serial.print("Encodeur G: ");
    Serial.println(encodeurG);
    Serial.print("Encodeur D: ");
    Serial.println(encodeurD);

    // Si un obstacle est détecté, on annule le déplacement pour sécuriser le robot.
    detecterObstacle();
    if (ObstacleDetecte == true) {
      annulerMouvement();
      ObstacleDetecte = false;
      break;
        }
    
    // Première réduction de vitesse lorsqu'on est à environ 700 ticks de la fin.
    if (!ralentissement1 &&
        encodeurG >= CibleG -700 &&
        encodeurD >= CibleD -700) {
      MOTOR_SetSpeed(LEFT, vitesse_Gauche*0.7);
      MOTOR_SetSpeed(RIGHT, vitesse_Droite*0.7);
      ralentissement1 = true;
    }

    // Deuxième ralentissement plus fort, proche de l'objectif final.
    if (!ralentissement2 &&
        encodeurG >= CibleG -400 &&
        encodeurD >= CibleD -400) {
      MOTOR_SetSpeed(LEFT, vitesse_Gauche*0.35);
      MOTOR_SetSpeed(RIGHT, vitesse_Droite*0.35);
      ralentissement2 = true;
    }

    delay(1);
  }

  arret();
}



// -----------------------------------------------------------------------------
// Fait pivoter le robot d'un angle donné en degrés.
// On convertit l'angle en distance approximative de roue, puis on applique des
// vitesses opposées sur les moteurs pour tourner autour du centre du robot.
// -----------------------------------------------------------------------------
void tourne(float angleDegres) { 
  float distanceRoue = (abs(angleDegres) * CIRCONFERENCE_TOURNER) / 360; // distance parcourue par chaque roue pour tourner de angleDegres

  long CibleG = distanceEnTicks(distanceRoue, 0);
  long CibleD = distanceEnTicks(distanceRoue, 1);

  ENCODER_Reset(0);
  ENCODER_Reset(1);




  // Sens de rotation : angle positif = droite, négatif = gauche.
  if (angleDegres > 0) { // Rotation vers la droite
    MOTOR_SetSpeed(LEFT, vitesse_Gauche_Tourne);
    MOTOR_SetSpeed(RIGHT, -vitesse_Droite_Tourne);
  } else { // Rotation vers la gauche
    MOTOR_SetSpeed(LEFT, -vitesse_Gauche_Tourne);
    MOTOR_SetSpeed(RIGHT, vitesse_Droite_Tourne);
  }

  // La rotation continue tant que les deux roues n'ont pas parcouru la distance
  // calculée pour atteindre l'angle demandé.
  while (abs(ENCODER_Read(0)) < CibleG && abs(ENCODER_Read(1)) < CibleD) {
    delay(1);
  }

  arret();

}


// -----------------------------------------------------------------------------
// Annule un mouvement en cours après la détection d'un obstacle.
// Le robot s'arrête, recule brièvement, puis augmente progressivement sa vitesse
// de recul pour sortir proprement de la zone bloquée.
// -----------------------------------------------------------------------------
void annulerMouvement() {

  arret();
  delay(300);

  // On mémorise la position avant le recul pour contrôler la distance de sortie.
  int EncodeurInitialG = ENCODER_Read(0);
  int EncodeurInitialD = ENCODER_Read(1);
  long CibleG = 700;
  long CibleD = 700;

  // Recul initial plus doux pour ne pas donner un coup de frein brutal.
  MOTOR_SetSpeed(LEFT, -vitesse_Gauche*0.35);
  MOTOR_SetSpeed(RIGHT, -vitesse_Droite*0.3);

  bool fast = false;
  bool topspeed = false;

  // Le recul se poursuit jusqu'à ce que les deux roues aient parcouru suffisamment.
  while (ENCODER_Read(0) > CibleG && ENCODER_Read(1) > CibleD) {
    
    long encodeurG = ENCODER_Read(0);
    long encodeurD = ENCODER_Read(1);

     // On passe à un recul plus rapide après 250 ticks de séparation.
     if (!fast &&
        encodeurG <= EncodeurInitialG - 250 &&
        encodeurD <= EncodeurInitialD - 250) {
      MOTOR_SetSpeed(LEFT, -vitesse_Gauche*0.7);
      MOTOR_SetSpeed(RIGHT, -vitesse_Droite*0.6);
      fast = true;
    }

    // La dernière étape de recul augmente encore la puissance pour sortir plus vite.
    if (!topspeed &&
        encodeurG <= EncodeurInitialG - 500 &&
        encodeurD <= EncodeurInitialD - 500) {
      MOTOR_SetSpeed(LEFT, -vitesse_Gauche*1.2);
      MOTOR_SetSpeed(RIGHT, -vitesse_Droite*1);
      topspeed = true;
    }

    delay(1);
  }
  arret();
  ObstacleDetecte = false;

}