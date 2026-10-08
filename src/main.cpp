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
#include <Parcours.h>
#include "Son.h"

// L'exploration ne commence qu'apres le signal sonore de depart.
bool robotDemarre = false;

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
  initialiserParcours();
}

void loop() {
  // Attend le signal de depart et maintient le robot a l'arret entre-temps.
  if (!robotDemarre) {
    arret();
    if (detecter5kHz()) {
      robotDemarre = true;
      beep(2);
    }
    return;
  }

  // Une etape peut sonder plusieurs directions bloquees, mais ne deplace le
  // robot que d'une case au maximum avant de rendre la main a loop().
  if (!parcoursTermine()) {
    parcourirUneEtape();
    if (parcoursTermine()) {
      arret();
      beep(parcoursReussi() ? 3 : 5);
    }
  } else {
    arret();
  }
}
