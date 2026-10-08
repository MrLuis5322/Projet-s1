#ifndef PARCOURS_H
#define PARCOURS_H

#define PARCOURS_LIGNES 10
#define PARCOURS_COLONNES 3

// Indicateurs stockes dans chaque case de parcours.
#define PARCOURS_VISITEE 0x0001
#define PARCOURS_COURANTE 0x0002
#define PARCOURS_TESTE_NORD 0x0004
#define PARCOURS_TESTE_EST 0x0008
#define PARCOURS_TESTE_SUD 0x0010
#define PARCOURS_TESTE_OUEST 0x0020
#define PARCOURS_BLOQUE_NORD 0x0040
#define PARCOURS_BLOQUE_EST 0x0080
#define PARCOURS_BLOQUE_SUD 0x0100
#define PARCOURS_BLOQUE_OUEST 0x0200

// Carte 10 x 3 : indique les cases visitees, la case courante et les passages
// deja testes; un passage bloque possede aussi son indicateur de direction.
extern int tableauParcours[PARCOURS_LIGNES][PARCOURS_COLONNES];

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
