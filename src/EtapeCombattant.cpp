#include <math.h>
#include <LibRobus.h>
#include <Mouvement.h>
#include <pince.h>
#include <DetecteurSuiveurLigne.h>
#include <Son.h>


void continuer() {

    int cas = detecterPositionRobot();

    if (cas == 2) avance(1);
    while (cas == 2){
        avanceMaj();
        cas = detecterPositionRobot();
        delay(10);
    }
    avancePlus();

    while (cas == 0 || cas == 7 || cas == 4 || cas == 6){
        tourneUneRoue(1, 1);
        cas = detecterPositionRobot();
        delay(10);
    }
    arret();

    while (cas == 1 || cas == 3){
        tourneUneRoue(-1, 1);
        cas = detecterPositionRobot();
        delay(10);
    }
    arret();
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
    const uint32_t CODE_REVEIL = 0x08F740BFUL;
    arret();

    while (true) {
        const uint32_t message = REMOTE_read();
        if (message == CODE_REVEIL) {
            beep(3);
            return;
        }
        delay(10);
    }
}

void EtapeVert () {
    // On espère qu'il va en ligne droite sinon Tweak la LIGNE 30,32 DE Mouvement.cpp
 delay(150);
 mouvementAvant(85);
}
