/*
Main - P13
Auteurs: Alexandre H., Luis 
Projet: Labyrinthe-S1
Equipe: 13-A
Auteurs: 
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

const int vertPin = 48;
const int rougePin = 49;

//********VARIABLES********//
bool bumperArr; //variable vrai ou faux
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
int count = 0;

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
  initialiserSon();
  initialiserDetecteurProximite();
  
  pinMode(vertPin, INPUT);
  pinMode(rougePin, INPUT);
  delay(100);
  Serial.println("Initialisation complete");
  beep(3);

//test
  Serial.begin(9600);

}

/*
Fonctions de boucle infini
 -> Se fait appeler perpetuellement suite au "setup"
*/
void loop() {

  //Test encodeurs PID

  if(ROBUS_IsBumper(3)){
    etat = 1;
  }
  if(ROBUS_IsBumper(2)){
    etat = 0;
    arret();
  }
  if(!etat){
    return;
  }

  avanceDroit();


  // etatPast = etat;
  // bumperArr = ROBUS_IsBumper(3);
  // if (bumperArr){
  //   if (etat == 0){
  //     beep(2);
  //     etat = 1;
  //   } 
  //   else{
  //     beep(1);
  //     etat = 0;
  //   }
  // }
  
  // vert = digitalRead(vertPin);
  // rouge = digitalRead(rougePin);
  // if (etat > 0){
  //   if (vert && rouge){ // aucun obstacle => avance
  //     etat = 1;
  //   }
  //   if (!vert && !rouge){  // obstacle devant => recule
  //     etat = 2;
  //   }
  //   if (!vert && rouge){ // obstacle à gauche => tourne droit
  //       etat = 3;
  //     }
  //   if (vert && !rouge){ // obstacle à droite => tourne gauche
  //       etat = 4;
  //   }
  // }

  // if (etatPast != etat){
  //   arret();
  //   delay(50);
  // }
  // else{
  //   switch (etat)
  //   {
  //   case 0:
  //     arret();
  //     break;
  //   case 1:
  //     avance();
  //     break;
  //   case 2:
  //     recule();
  //     break;
  //   case 3:
  //     tourneDroit();
  //     break;
  //   case 4:
  //     tourneGauche();
  //     break;            
  //   default:
  //     avance();
  //     etat = 1;
  //   break;
  //   }
  // }
  // delay(200);
}