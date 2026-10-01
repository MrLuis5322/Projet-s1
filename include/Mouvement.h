#ifndef MOUVEMENT_H
#define MOUVEMENT_H

extern int etat;
extern int etatPast;
extern float vitesse_Gauche;
extern float vitesse_Droite;

void arret();
void avance();
void recule();
void tourneDroite90();
void tourneGauche90();
void mouvementAvant(float temps);
void mouvementArriere(float temps);
#endif
