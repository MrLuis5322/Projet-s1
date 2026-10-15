#ifndef PARCOURS_H
#define PARCOURS_H

#define PARCOURS_LIGNES 10
#define PARCOURS_COLONNES 3

// Chaque case contient 0 si elle n'a pas ete visitee et 1 si elle l'a ete.
// Le robot est a la case dont la valeur est 2.
extern int tableauParcours[PARCOURS_LIGNES][PARCOURS_COLONNES];

// Chaque tableau contient des 0 et des 1 : l'un note les directions examinees,
// l'autre les directions bloquees. Ordre : nord, est, sud, ouest.
extern int directionsExaminees[PARCOURS_LIGNES][PARCOURS_COLONNES][4];
extern int passagesBloques[PARCOURS_LIGNES][PARCOURS_COLONNES][4];

// Initialise la carte et place virtuellement le robot au depart (case du bas,
// colonne centrale), avec l'avant du robot oriente vers le haut du parcours.
void initialiserParcours();

// Effectue une etape de recherche avec le capteur avant.
// Les appels successifs font avancer ou revenir le robot jusqu'a la fin.
void parcourirUneEtape();

// Indique si l'algorithme a termine, avec succes ou parce qu'aucune route
// exploitable n'a ete trouvee.
bool parcoursTermine();

// Indique si la fin a ete atteinte dans une des trois cases de la rangee haute.
bool parcoursReussi();

#endif
