#ifndef MOUVEMENT_H
#define MOUVEMENT_H

extern int etat;
extern int etatPast;
extern float vitesse_Gauche;
extern float vitesse_Droite;

void arret();
void tourne(float angleDegres);
void mouvementAvant(float DistanceCM);

#endif