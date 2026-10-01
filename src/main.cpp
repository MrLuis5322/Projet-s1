/*
Projet: Main - P13
Auteurs: Alexandre H., Luis
Description: Code pour sortie du labyrinthe
Date : 01/10/2026
*/

/*
Inclure les librairies de functions que vous voulez utiliser
*/

#include <LibRobus.h>
#include <Mouvement.h>
#include <DetecteurProximite.h>

/*
Variables globales et defines
*/
int posX = 2; // 1,2,3
int posY = 1; // 1 - 10 (Goal)
bool goal = false; //

bool bumperGa; //0
bool bumperDr; //1
bool bumperAv; //2
bool bumperArr; //3

int etat = 0; // = 0=Arret 1=Avance 2=Droite 3=Gauche 4=Recule 
int etatPast = 0;


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
*/

void setup(){
  BoardInit();
  initialiserDetecteurProximite();
  Serial.begin(115200);
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(1000);
  beep(1);
}

/*
Fonctions de boucle infini
*/


void loop() {

  while (goal != true) {
    Serial.println("Goal: ");

    while (ObstacleDetecte == false) {
      Serial.println(ObstacleDetecte);
        mouvementAvant(250);
        delay(5000);
      }
    
  }
}

  



