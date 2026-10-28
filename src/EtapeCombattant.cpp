#include <math.h>
#include <LibRobus.h>
#include <Mouvement.h>


void EtapeJaune () {
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
}

void EtapeRouge () { 
    mouvementAvant(18);
    delay(250);
    // Turn right 90 degrees to start circular path
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
    // move on

    }
// A MODIFIER 
void EtapeOrange () {
   mouvementAvant(18);
    delay(250);
    // Turn right 90 degrees to start circular path
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

}

void EtapeBleu () {
arret();
// Attendre message IR
}

void EtapeVert () {
 delay(150);
 mouvementAvant(85);
}
