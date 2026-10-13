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

void setup(){
  BoardInit();
  
  pinMode(vertPin, INPUT);
  pinMode(rougePin, INPUT);

  InitialiserMemoire();
  initialiserSon();
  delay(100);
  Serial.println("Initialisation complete");
}

void loop() {

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

