/*
Projet: Mouvement
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Code pour faire bouger et tourner le robot
Date: 24/09/2026
*/

#include <LibRobus.h>
#include <Mouvement.h>


// Etat et reglages partages avec main.cpp et DetercteurProximite.cpp.
// Le header les declare avec extern; ces lignes en sont les definitions uniques.
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse_Gauche = 0.40;
float vitesse_Droite = 0.427;


// Arrete les deux moteurs en envoyant une vitesse nulle.
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};

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

// Pour tourner sur place, les moteurs tournent dans des directions opposees.
void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse_Droite);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse_Gauche);
};

// Meme principe que tourneDroit, avec les directions inversees.
void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse_Droite);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse_Gauche);
};