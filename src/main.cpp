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
#define EN_MARCHE 1
#define ARRET 0

const int vertPin = 48;
const int rougePin = 49;


//********VARIABLES********//
bool bumperArr; //variable vrai ou faux
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;

//********FONCTIONS********//
/*
void setup(){
  BoardInit();
  
  pinMode(vertPin, INPUT);
  pinMode(rougePin, INPUT);
  delay(100);
  Serial.println("Initialisation complete");
}

void loop() {

  //Test encodeurs PID
  if(ROBUS_IsBumper(3)){
    etat = EN_MARCHE;
  }
  if(ROBUS_IsBumper(2)){
    etat = ARRET;
    arret();
  }
  if(!etat){
    return;
  }

  encLeft = ENCODER_Read(LEFT);
  encRight = ENCODER_Read(RIGHT);

  avanceDroit();

  countEncLeft += encLeft;
  countEncRight += encRight;

  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);
  delay(10);
}

*/