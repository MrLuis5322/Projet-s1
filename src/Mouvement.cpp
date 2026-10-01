/*
Mouvement - P13
Alexandre H.
Code pour faire bouger et tourner le robot
24/09/2026
*/

#include <LibRobus.h>
#include <Mouvement.h>


float SpeedMult = 1.5;
float vitesse_Gauche = 0.40*SpeedMult;
float vitesse_Droite = 0.426*SpeedMult;

int temps = 1000; // temps de déplacement en ms

namespace {
const float KP = 1.0;
const float KI = 0.40;
const float KD = 0.15;
const float dt = 0.01;

float vitesseLeft = 0.0;
float vitesseRight = 0.0;
const float pulseCibleLeft = 50.63;
const float pulseCibleRight = 50.0;
float integrale = 0.0;
float erreurPrecedenteLeft = 0.0;
float erreurPrecedenteRight = 0.0;



float calculPid(int32_t pulse, float vitesse, float &integrale,
                float &erreurPrecedente, float pulseCible) {
  float erreur = pulseCible - pulse;
  integrale += erreur * dt;
  float derivee = (erreur - erreurPrecedente) / dt;
  float correction = KP * erreur + KI * integrale + KD * derivee;
  erreurPrecedente = erreur;

  return vitesse + correction * 0.0001;
}
}






// Arrete les deux moteurs en envoyant une vitesse nulle.
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
}

void avance(){
  MOTOR_SetSpeed(LEFT, vitesseLeft);
  MOTOR_SetSpeed(RIGHT, vitesseRight);
}

void recule(){
  MOTOR_SetSpeed(LEFT, -0.5*vitesseLeft);
  MOTOR_SetSpeed(RIGHT, -0.5*vitesseRight);
}

void tourneDroit(){
  MOTOR_SetSpeed(LEFT, 0.5*vitesseLeft);
  MOTOR_SetSpeed(RIGHT, -0.5*vitesseRight);
}

void tourneGauche(){
  MOTOR_SetSpeed(LEFT, -0.5*vitesseLeft);
  MOTOR_SetSpeed(RIGHT, 0.5*vitesseRight);
}

void avanceDroit(){
  int32_t encLeft = ENCODER_Read(LEFT);
  int32_t encRight = ENCODER_Read(RIGHT);
  avance();
  vitesseLeft = calculPid(encLeft, vitesseLeft, integrale,
                          erreurPrecedenteLeft, pulseCibleLeft);
  vitesseRight = calculPid(encRight, vitesseRight, integrale,
                           erreurPrecedenteRight, pulseCibleRight);

  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);
  delay(10);
}
/* Ancien avant arriere 
// Une vitesse positive fait avancer chaque moteur.
void avance(){
  MOTOR_SetSpeed(RIGHT,vitesse_Droite);
  MOTOR_SetSpeed(LEFT, vitesse_Gauche);
};

// Les vitesses negatives inversent le sens des moteurs.
// Le facteur 0.52 compense la difference de comportement du robot en reculant.
void recule(){
  MOTOR_SetSpeed(RIGHT, -vitesse_Droite);
  MOTOR_SetSpeed(LEFT, -0.52*vitesse_Gauche);
};
*/

// Pour tourner sur place d'environ 90 degrees a sa droite.
void tourneDroite90(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse_Droite);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse_Gauche);
  delay(890);
  arret();
};

// Pour tourner sur place d'environ 90 degrees a sa gauche.
void tourneGauche90(){
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse_Droite);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse_Gauche);
  delay(910);
  arret();
};

void mouvementAvant(float temps){
  MOTOR_SetSpeed(RIGHT, vitesse_Droite);
  MOTOR_SetSpeed(LEFT, vitesse_Gauche);
  delay(temps); 
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse_Droite); // slow stop 
  MOTOR_SetSpeed(LEFT, 0.5*vitesse_Gauche);
  delay(150);
  arret();
};
