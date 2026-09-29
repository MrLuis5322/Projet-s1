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


// Arrete les deux moteurs en envoyant une vitesse nulle.
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
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
