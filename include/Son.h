#ifndef SON_H
#define SON_H


const int Seuil_Tension = 500; //  seuil  d activation 
const long long Duree_Confirmation_ms = 50; //Dure de la confirmation pour que le robot ne demare pas avec un son random
const int PIN_SON = A1; //pin sur Arduino robot B 


// Joue un nombre donne de bips courts.
void beep(int count);

// Initialise le module sonore.
void initialiserSon();

bool detecter5kHz();

# endif