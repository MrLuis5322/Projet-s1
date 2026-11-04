#include <math.h>
#include <LibRobus.h>
#include <Mouvement.h>


void continuer(){
    
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
// A MODIFIER 
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
