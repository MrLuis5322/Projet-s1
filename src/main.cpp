/*
Projet: Le nom du script
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/

/*
Inclure les librairies de functions que vous voulez utiliser
*/
#include <LibRobus.h>
#include <DetercteurProximite.h>
#include <Mouvement.h>
#include <Son.h>

/*
Variables globales et defines
 -> defines...
 -> L'ensemble des fonctions y ont acces
*/

bool bumperArr;


/*
Fonctions d'initialisation (setup)
 -> Se fait appeler au debut du programme
 -> Se fait appeler seulement un fois
 -> Generalement on y initilise les varibbles globales
*/
void setup(){
  BoardInit();
  
  initialiserDetecteurProximite();
  delay(100);
  beep(3);
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
  
  mettreAJourEtatAvecDetecteurs();

  if (etatPast != etat){
    arret();
    delay(50);
  }
  else{
    switch (etat)
    {
    case ETAT_ARRET:
      arret();
      break;
    case ETAT_AVANCE:
      avance();
      break;
    case ETAT_RECULE:
      recule();
      break;
    case ETAT_TOURNE_DROIT:
      tourneDroit();
      break;
    case ETAT_TOURNE_GAUCHE:
      tourneGauche();
      break;            
    default:
      avance();
      etat = 1;
    break;
    }
  }
  delay(200);
}