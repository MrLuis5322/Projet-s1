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
#include "Algorithme.h"
#include "Son.h"

//********CONSTANTES*******//
#define EN_MARCHE true
#define ARRET false

const int vertPin = 48;
const int rougePin = 49;


//********VARIABLES********//
bool bumperArr; //variable vrai ou faux
bool vert = false;
bool rouge = false;
int etat = false; 

//********FONCTIONS********//

void setup(){
  BoardInit();
  
  pinMode(vertPin, INPUT);
  pinMode(rougePin, INPUT);

  InitialiserAlgorithme();
  initialiserSon();
  delay(100);
  Serial.println("Initialisation complete");
}

void loop() {

  if(etat) etat = GetEtat();

  //Test encodeurs PID
  if(detecter5kHz()){
    etat = EN_MARCHE;
  }
  if(ROBUS_IsBumper(2)){
    etat = ARRET;
    arret();
  }
  if(!etat){
    return;
  }

  Devant();
  delay(10);
}

