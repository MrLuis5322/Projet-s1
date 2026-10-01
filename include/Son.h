#ifndef SON_H
#define SON_H


const int Seuil_Tension = 500; //  seuil  d activation 
const int PIN_SON = A1; //pin sur Arduino robot B 
// Joue un nombre donne de bips courts.
void beep(int count);

// Initialise le module sonore.
void initialiserSon();

bool detecter5kHz();

#endif
