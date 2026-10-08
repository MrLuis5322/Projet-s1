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


// Arrete les deux moteurs en envoyant une vitesse nulle.
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
}

long distanceEnTicks(float distanceCM, int encodeur) {
  if (encodeur == 0) {
    return lround(distanceCM / CM_PAR_TICK_G);
  }
  return lround(distanceCM / CM_PAR_TICK_D);
}


void mouvementAvant(float distanceCM) {
  long CibleG = distanceEnTicks(distanceCM, 0);
  long CibleD = distanceEnTicks(distanceCM, 1);

  ENCODER_Reset(0);
  ENCODER_Reset(1);
  
  MOTOR_SetSpeed(LEFT, vitesse_Gauche);
  MOTOR_SetSpeed(RIGHT, vitesse_Droite);


  bool ralentissement1 = false;
  bool ralentissement2 = false;

  while (ENCODER_Read(0) < CibleG && ENCODER_Read(1) < CibleD) {
    
    long encodeurG = ENCODER_Read(0);
    long encodeurD = ENCODER_Read(1);
    Serial.print("Encodeur G: ");
    Serial.println(encodeurG);
    Serial.print("Encodeur D: ");
    Serial.println(encodeurD);
    detecterObstacle();
    if (ObstacleDetecte == true) {
      annulerMouvement();
      ObstacleDetecte = false;
      break;
        }
    
    if (!ralentissement1 &&
        encodeurG >= CibleG -700 &&
        encodeurD >= CibleD -700) {
      MOTOR_SetSpeed(LEFT, vitesse_Gauche*0.7);
      MOTOR_SetSpeed(RIGHT, vitesse_Droite*0.7);
      ralentissement1 = true;
    }
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



void tourne(float angleDegres) { 
  float distanceRoue = (abs(angleDegres) * CIRCONFERENCE_TOURNER) / 360; // distance parcourue par chaque roue pour tourner de angleDegres

  long CibleG = distanceEnTicks(distanceRoue, 0);
  long CibleD = distanceEnTicks(distanceRoue, 1);

  ENCODER_Reset(0);
  ENCODER_Reset(1);




  // Tourne gauche ou droite
  if (angleDegres > 0) {     // Droite
    MOTOR_SetSpeed(LEFT, vitesse_Gauche_Tourne);
    MOTOR_SetSpeed(RIGHT, -vitesse_Droite_Tourne);
  } else {                   // Gauche
    MOTOR_SetSpeed(LEFT, -vitesse_Gauche_Tourne);
    MOTOR_SetSpeed(RIGHT, vitesse_Droite_Tourne);
  }

while (abs(ENCODER_Read(0)) < CibleG && abs(ENCODER_Read(1)) < CibleD) {
    delay(1);
  }

  arret();

}


void annulerMouvement() {

  arret();
  delay(300);
  int EncodeurInitialG = ENCODER_Read(0);
  int EncodeurInitialD = ENCODER_Read(1);
  long CibleG = 700;
  long CibleD = 700;
  
  MOTOR_SetSpeed(LEFT, -vitesse_Gauche*0.35);
  MOTOR_SetSpeed(RIGHT, -vitesse_Droite*0.3);

  bool fast = false;
  bool topspeed = false;

  while (ENCODER_Read(0) > CibleG && ENCODER_Read(1) > CibleD) {
    
    long encodeurG = ENCODER_Read(0);
    long encodeurD = ENCODER_Read(1);

     if (!fast &&
        encodeurG <= EncodeurInitialG - 250 &&
        encodeurD <= EncodeurInitialD - 250) {
      MOTOR_SetSpeed(LEFT, -vitesse_Gauche*0.7);
      MOTOR_SetSpeed(RIGHT, -vitesse_Droite*0.6);
      fast = true;
    }
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