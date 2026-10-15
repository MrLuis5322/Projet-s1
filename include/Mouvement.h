#ifndef MOUVEMENT_H
#define MOUVEMENT_H
//**************************************
// Projet: Labyrinthe-S1
// Fichiers: Mouvement.cpp
// Equipe: 13-A
// Auteurs: 
// Description: Breve description du script
// Date: 24/09/2026
//***************************************

//*********DEFINES********//
#define KP_AVANCE 1
#define KI_AVANCE 0.40
#define KD_AVANCE 0.15

#define KP_ARRET 2
#define KI_ARRET 0.4
#define KD_ARRET 0.3

#define KP_TOURNE 0.001
#define KI_TOURNE 0.001
#define KD_TOURNE 0.025

#define dt 0.01

#define FINI 0
#define ENCOURS 1
#define PULSE_CIBLE_LEFT 60.53
#define PULSE_CIBLE_RIGHT 60

//*****VARIABLES GLOBALES*****//
extern int32_t countEncLeft;
extern int32_t countEncRight;
extern int32_t encLeft;
extern int32_t encRight;
extern float vitesseLeft;
extern float vitesseRight;
//********FONCTIONS********//
void arret(void);
float calculPid(int pulse, float vitesse, float &erreurPrecedente, float pulseCible);
void avanceDroit(void);
void retourFinal(void);
void arretPID(float distance);
void rotation90(int direction);
void avance(float distance);
#endif