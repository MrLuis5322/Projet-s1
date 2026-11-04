/*
Projet: Main - P13
Auteurs: Alexandre H., Luis
Description: Code pour sortie du labyrinthe
Date : 01/10/2026


Rose | R: 60.00 a 61.00 (moy 60.41) | G: 20.00 a 21.00 (moy 20.97) | B: 35.00 a 35.00 (moy 35.00)
Place le capteur sur : Jaune
Jaune | R: 74.00 a 75.00 (moy 74.45) | G: 50.00 a 51.00 (moy 50.21) | B: 24.00 a 24.00 (moy 24.00)
Place le capteur sur : Orange
Orange | R: 56.00 a 57.00 (moy 56.97) | G: 12.00 a 13.00 (moy 12.97) | B: 9.00 a 9.00 (moy 9.00)
Place le capteur sur : Bleu
Bleu | R: 8.00 a 8.00 (moy 8.00) | G: 26.00 a 27.00 (moy 26.10) | B: 50.00 a 51.00 (moy 50.34)
Place le capteur sur : Vert
Vert | R: 6.00 a 6.00 (moy 6.00) | G: 13.00 a 13.00 (moy 13.00) | B: 5.00 a 6.00 (moy 5.03)


*/

/*
Inclure les librairies de functions que vous voulez utiliser
*/

#include <LibRobus.h>
#include <Mouvement.h>
#include <DetecteurProximite.h>
#include <Parcours.h>
#include "Son.h"
#include <EtapeCombattant.h>
#include <traitement_couleur.h>
/*
Variables globales et defines
*/
// L'exploration ne commence qu'apres le signal sonore de depart.
bool robotDemarre = false;
bool bumperGa; //0
bool bumperDr; //1
bool bumperAv; //2
bool bumperArr; //3

int etat = 0; // = 0=Arret 1=Avance 2=Droite 3=Gauche 4=Recule 
int etatPast = 0;

// Confirmation des lectures pour eviter plusieurs sequences de bips
// lorsque le capteur oscille entre deux couleurs.
CouleurId couleurCandidate = AUCUN;
CouleurId derniereCouleurAnnoncee = AUCUN;
int lecturesCouleurStables = 0;

/*
Fonctions d'initialisation (setup)
*/



void setup(){
  BoardInit();
  initialiserDetecteurProximite();
  Serial.begin(115200);
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  delay(1000);
  beep(1);
  initialiserSon();
  initialiserCapteurCouleur();

  // Les bumpers sont lus dans loop(); on commence sans mouvement.
  arret();
}

void loop() {

  CouleurId couleurLue = identifierCouleur(lireCapteur());
  if (couleurLue == couleurCandidate) {
    if (lecturesCouleurStables < 3) lecturesCouleurStables++;
  } else {
    couleurCandidate = couleurLue;
    lecturesCouleurStables = 1;
  }

  if (lecturesCouleurStables >= 8 && couleurCandidate != derniereCouleurAnnoncee) {
    int nombreBips = 0;
    switch (couleurCandidate) {
      case ROSE:   nombreBips = 1; break;
      case JAUNE:  nombreBips = 2; break;
      case ORANGE: nombreBips = 3; break;
      case BLEU:   nombreBips = 4; break;
      case VERT:   nombreBips = 5; break;
      default:     nombreBips = 0; break;
    }

    if (nombreBips > 0) beep(nombreBips);
    derniereCouleurAnnoncee = couleurCandidate;
  }

  bool Arr = ROBUS_IsBumper(3);
  if (Arr) {
    delay(500);
    mouvementAvant(150.0f);
    arret();
    }

  bool Gauche = ROBUS_IsBumper(0);
  if (Gauche) {
    delay(1000);
    tourne(-90);
  }
  // Bumper 1 : lance la calibration des couleurs
  bool droite = ROBUS_IsBumper(1);
  if (droite) {
    delay(500);          
    modeCalibration();
  }
  bool Av = ROBUS_IsBumper(2);
    if (Av) {
      delay(500);
      EtapeRouge();
    }
}

