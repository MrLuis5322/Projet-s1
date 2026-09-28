#ifndef SON_H
#define SON_H


const int Seuil_Tension = 600; // Exemple de seuil random a ajuster
const int PIN_SON = A0; //pas la bonne pin a changer

// Joue un nombre donne de bips courts.
void beep(int count);

// Initialise le module sonore.
void initialiserSon();

bool detecter5kHz();

#endif
