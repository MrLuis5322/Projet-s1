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
<<<<<<< HEAD
#include "DetecteurProximite.h"
#include "Mouvement.h"
#include "Son.h"
=======
#include <DetecteurProximite.h>
#include <Mouvement.h>
#include <Son.h>
>>>>>>> a5909f77e9992378e785f97819175e2ea3322686

/*
Variables globales et defines
 -> defines...
 -> L'ensemble des fonctions y ont acces
*/

bool bumperGa; //0
bool bumperDr; //1
bool bumperAv; //2
bool bumperArr; //3

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
// Tourner a gauche
   bumperGa = ROBUS_IsBumper(0);
  if (bumperGa){
      tourneGauche90();
  }
// Tourner a droite
  bumperDr = ROBUS_IsBumper(1);
  if (bumperDr){
      tourneDroite90();
  }
  // Test avancer pour determiner la vitesse
  bumperAv = ROBUS_IsBumper(2);
  if (bumperAv){
      mouvementAvant(1000);
  }

  mettreAJourEtatAvecDetecteurs();

  if (etatPast != etat){
    arret();
    delay(50);
  }
  else{
    switch (etat)
    {
    case 0:
      arret();
      break;
    case 1:
      mouvementAvant(1000);
      break;
    case 2:
      mouvementArriere(1000);
      break;
    case 3:
      tourneDroite90();
      break;
    case 4:
      tourneGauche90();
      break;            
    default:
      mouvementAvant(1000);
      etat = 1;
    break;
    }
  }
  delay(200);

  Serial.print("Valeur lue: ");
  Serial.println((analogRead(PIN_SON)));
}





/*
Au calme (sans son 5 kHz) : notez la valeur moyenne (ex: 50 ou 100).
Avec le son 5 kHz allumé à distance réelle : notez la valeur (ex: 750).
Choisir un soeuil a michemin


*/