/*
Main - P13
Auteurs: Alexandre H., Luis 
Description: Breve description du script
Date: 29/09/2026
*/

/*
librairies de functions a utiliser
*/
#include <Arduino.h>
#include <LibRobus.h>
#include "DetecteurProximite.h"
#include "Mouvement.h"
#include "Son.h"

/*
Variables globales et defines
 -> defines...
 -> L'ensemble des fonctions y ont acces
*/

bool bumperGa; //0
bool bumperDr; //1
bool bumperAv; //2
bool bumperArr; //3

int posX = 0;
int posY = 0;

int vertpin = 48;
int rougepin = 49; 
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;

// Etat et reglages partages avec main.cpp et DetercteurProximite.cpp.
// Le header les declare avec extern; ces lignes en sont les definitions uniques.
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
/*
Fonctions d'initialisation (setup)
 -> Se fait appeler au debut du programme
 -> Se fait appeler seulement un fois
 -> Generalement on y initilise les varibbles globales
*/

void setup(){
  BoardInit();
  initialiserSon();
  initialiserDetecteurProximite();
  delay(100);
  beep(3);

//test
  Serial.begin(9600);

}

/*
Fonctions de boucle infini
 -> Se fait appeler perpetuellement suite au "setup"
*/

  Serial.print("Valeur lue: ");
  Serial.println((analogRead(PIN_SON)));

/*
Vos propres fonctions sont creees ici
*/

void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}
/*
Fonctions d'initialisation (setup)
 -> Se fait appeler au debut du programme
 -> Se fait appeler seulement un fois
 -> Generalement on y initilise les varibbles globales
*/
void setup(){
  BoardInit();
  Serial.begin(115200);
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  //initialisation
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);
  beep(1);
}

/*
Fonctions de boucle infini
 -> Se fait appeler perpetuellement suite au "setup"
*/


void loop() {

  etatPast = etat;
  bumperArr = ROBUS_IsBumper(3);
  if (bumperArr){
    if (etat == 0){
      beep(2);
      etat = 1;
    } 
    else{
      beep(1);
      etat = 0;
    }
  }

  bumperGa = ROBUS_IsBumper(0);
  if (bumperGa){
   for (int i = 0; i < 4; i++) {
      tourne(-90);
      delay(500);
  }
 }
  bumperDr = ROBUS_IsBumper(1);
  if (bumperDr){
    for (int i = 0; i < 4; i++) {
      tourne(90);
      delay(500);
    }
  }
  
  bumperAv = ROBUS_IsBumper(2);
  if (bumperAv){
    mouvementAvant(50);
  }

  vert = digitalRead(vertpin);
  rouge = digitalRead(rougepin);
  if (etat > 0){
    if (vert && rouge){ // aucun obstacle => avance
      etat = 1;
    }
    if (!vert && !rouge){  // obstacle devant => recule
      etat = 2;
    }
    if (!vert && rouge){ // obstacle à gauche => tourne droite
        etat = 3;
      }
    if (vert && !rouge){ // obstacle à droite => tourne gauche
        etat = 4;
    }
  }

  if (etatPast != etat){
    arret();
    delay(50);
  }
}



