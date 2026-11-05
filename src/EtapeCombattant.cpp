#include <math.h>
#include <LibRobus.h>
#include <Mouvement.h>
#include <pince.h>
#include <DetecteurSuiveurLigne.h>


void continuer() {
    int cas = detecterPositionRobot();
    static int dernierCoteLigne = 0; // -1 gauche, +1 droite
    // Une seule mise à jour par appel afin que loop() continue de lire les
    // bumpers et le capteur de couleur.
    switch (cas) {
      case 2: // ligne au centre
      case 5: // intersection ou ligne large
      case 7:
        MOTOR_SetSpeed(LEFT, 0.30f);
        MOTOR_SetSpeed(RIGHT, 0.30f);
        break;
      case 1: // ligne à droite
      case 3:
        dernierCoteLigne = 1;
        MOTOR_SetSpeed(LEFT, 0.30f);
        MOTOR_SetSpeed(RIGHT, 0.16f);
        break;
      case 4: // ligne à gauche
      case 6:
        dernierCoteLigne = -1;
        MOTOR_SetSpeed(LEFT, 0.16f);
        MOTOR_SetSpeed(RIGHT, 0.30f);
        break;
      case 0: // ligne perdue / départ : avance lentement pour la retrouver
        if (dernierCoteLigne > 0) {
          MOTOR_SetSpeed(LEFT, 0.24f);
          MOTOR_SetSpeed(RIGHT, 0.12f);
        } else if (dernierCoteLigne < 0) {
          MOTOR_SetSpeed(LEFT, 0.12f);
          MOTOR_SetSpeed(RIGHT, 0.24f);
        } else {
          MOTOR_SetSpeed(LEFT, 0.18f);
          MOTOR_SetSpeed(RIGHT, 0.18f);
        }
        break;
      default: // ligne large / intersection
        MOTOR_SetSpeed(LEFT, 0.25f);
        MOTOR_SetSpeed(RIGHT, 0.25f);
        break;
    }
}

void EtapeJaune () {
    // Séquence de mouvements pour éviter le mur hard coded
delay(100);
tourne(90);
delay(300);
mouvementAvant(50);
delay(300);
tourne(-90);
delay(300);
mouvementAvant(50);
delay(300);
tourne(-90);
delay(300);
mouvementAvant(50);
delay(300);
tourne(90);
delay(300);
// move on - REPRISE DU CODE DU SUIVEUR DE LIGNE
}

void EtapeRouge () { 
    // Se déplace jusqu'au bout du cercle et fait le tour pour faire tomber la quille
    mouvementAvant(18);
    delay(250);
    tourne(90);
    delay(150);
    
    for (int i = 0; i < 12; ++i) {
        mouvementAvant(11.388);
        delay(150);
        tourne(28);
        delay(150);
    }        
    tourne(-89); 
    delay(150);
    // move on - REPRISE DU CODE DU SUIVEUR DE LIGNE

    }
void EtapeOrange () {
    // Se déplace jusqu'au bout du cercle et fait le tour, il devrait théoriquement avoir
    // le verre dans sa pince donc il faut attraper a la fin, mais je n'ai pas la fonction pince
   mouvementAvant(18);
    delay(250);
    tourne(90);
    delay(150);
    
    for (int i = 0; i < 12; ++i) {
        mouvementAvant(11.388);
        delay(150);
        tourne(28);
        delay(150);
    }        
    tourne(-89); // tweak l'angle si le capteur voit pas la ligne
    delay(150);
    // move on - REPRISE DU CODE DU SUIVEUR DE LIGNE
}

void EtapeBleu () {
arret();
// Attendre message IR
}

void EtapeVert () {
    // On espère qu'il va en ligne droite sinon Tweak la LIGNE 30,32 DE Mouvement.cpp
 delay(150);
 mouvementAvant(85);
}
