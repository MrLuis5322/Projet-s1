//**************************************
// Projet: Labyrinthe-S1
// Fichiers: Mouvement.cpp
// Equipe: 13-A
// Auteurs: 
// Description: Breve description du script
// Date: 24/09/2026
//***************************************

//*********INCLUDES********//
#include <LibRobus.h>
#include <Arduino.h>
#include "Mouvement.h"

//********CONSTANTES*******//
const int vertPin = 48;
const int rougePin = 49;

const float diametreRoue = 0.0762; // en mètre
const float pi = 3.14159;
const float DiametreRobot = 0.205; // en mètre
const float CirconferenceRoue = pi * diametreRoue; // en mètre

//********VARIABLES********//
float vitesseLeft = 0.0;
float vitesseRight = 0.0;
int32_t countEncLeft = 0;
int32_t countEncRight = 0;
int32_t encLeft = 0;
int32_t encRight = 0;

// variables PID
float Kp = 0.0;
float Ki = 0.0;
float Kd = 0.0;
float pulseCibleLeft = 60;
float pulseCibleRight = 60;
float integrale = 0;
float derivee = 0;
float correction = 0;
float erreurPrecedenteLeft = 0;
float erreurPrecedenteRight = 0;

//********FONCTIONS********//
void arret(){
  vitesseLeft = 0;
  vitesseRight = 0;
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
}

float calculPid(int pulse, float vitesse, float &erreurPrecedente, float pulseCible){
  float erreur = pulseCible - pulse;
  
  integrale += erreur * dt;
  derivee = (erreur - erreurPrecedente) / dt;
  correction = Kp * erreur + Ki * integrale + Kd * derivee;

  erreurPrecedente = erreur;

  return vitesse + correction*0.0001; 
}

void avanceDroit(void){
  Kp = KP_AVANCE;
  Ki = KI_AVANCE;
  Kd = KD_AVANCE;

  vitesseRight = calculPid(encRight, vitesseRight, erreurPrecedenteRight, pulseCibleRight);
  vitesseLeft = calculPid(encLeft, vitesseLeft, erreurPrecedenteLeft, pulseCibleLeft);

  MOTOR_SetSpeed(LEFT, vitesseLeft);
  MOTOR_SetSpeed(RIGHT, vitesseRight);
}

void arretPID(float distance){
  Kp = KP_ARRET;
  Ki = KI_ARRET;
  Kd = KD_ARRET;

  if(countEncLeft >= distance && countEncRight >= distance){
    arret();
  }
  else{
    vitesseLeft = calculPid(encLeft, vitesseLeft, erreurPrecedenteLeft, 0);
    vitesseRight = calculPid(encRight, vitesseRight, erreurPrecedenteRight, 0);

    MOTOR_SetSpeed(LEFT, vitesseLeft);
    MOTOR_SetSpeed(RIGHT, vitesseRight);
  }
}

void rotation90(int direction){
  Kp = KP_TOURNE;
  Ki = KI_TOURNE;
  Kd = KD_TOURNE;
  float distanceCible = (pi * DiametreRobot) /(4 * CirconferenceRoue); // Rayon de la roue = 5cm
  float pulseCible = distanceCible * 3200;

  if(direction == LEFT){
    pulseCibleLeft = -pulseCible;
    pulseCibleRight = pulseCible;
  }
  else if(direction == RIGHT){
    pulseCibleLeft = pulseCible;
    pulseCibleRight = -pulseCible;
  }
  else{
    return;
  }

  vitesseRight = calculPid(countEncRight, vitesseRight, erreurPrecedenteRight, pulseCibleRight);
  vitesseLeft = calculPid(countEncLeft, vitesseLeft, erreurPrecedenteLeft, pulseCibleLeft);

  MOTOR_SetSpeed(LEFT, vitesseLeft);
  MOTOR_SetSpeed(RIGHT, vitesseRight);
}

void avance(float distance){
  float distancePulse = distance / CirconferenceRoue * 3200;

  float pulseArret = distancePulse * 0.9;

  if(countEncLeft < pulseArret && countEncRight < pulseArret){
    avanceDroit();
  }
  if(countEncLeft >= pulseArret && countEncRight >= pulseArret){
    arretPID(distancePulse);
  }
}