#include <Arduino.h>
#include <Mouvement.h>
#include "Parcours.h"

// Le plan fait 5 m de haut repartis sur 10 cases de 50 cm.
static const float DISTANCE_ENTRE_CASES_CM = 50.0f;
static const int NOMBRE_DIRECTIONS = 4;
static const int NOMBRE_CASES =
    PARCOURS_LIGNES * PARCOURS_COLONNES;

enum Direction {
  NORD = 0,
  EST,
  SUD,
  OUEST
};

// La pile garde chaque case du chemin et les directions restant a examiner.
// Une case est codee par son indice dans le tableau 10 x 3.
static int pileIndicesCases[NOMBRE_CASES];
static int pileDirectionsRetour[NOMBRE_CASES];
static int pileProchainesDirections[NOMBRE_CASES];
static int sommetPile = -1;

static int ligneActuelle = PARCOURS_LIGNES - 1;
static int colonneActuelle = PARCOURS_COLONNES / 2;
static int orientationActuelle = NORD;
static bool parcoursInitialise = false;
static bool parcoursEstTermine = false;
static bool sortieAtteinte = false;

int tableauParcours[PARCOURS_LIGNES][PARCOURS_COLONNES];

// Calcule la case voisine dans la direction demandee.
static void trouverCaseVoisine(int ligne, int colonne, int direction, 
                              int *ligneVoisine, int *colonneVoisine) {
  *ligneVoisine = ligne;
  *colonneVoisine = colonne;

  switch (direction) {
    case NORD:
      --(*ligneVoisine);
      break;
    case EST:
      ++(*colonneVoisine);
      break;
    case SUD:
      ++(*ligneVoisine);
      break;
    default:
      --(*colonneVoisine);
      break;
  }
}

// Retourne le bit qui indique si une direction a deja ete examinee.
static int bitDirectionExaminee(int direction) {
  return PARCOURS_TESTE_NORD << direction;
}

// Retourne le bit qui indique si un mur a ete detecte dans cette direction.
static int bitDirectionBloquee(int direction) {
  return PARCOURS_BLOQUE_NORD << direction;
}

// Enregistre un passage dans les deux cases voisines pour garder la carte
// coherente, quel que soit le cote depuis lequel le passage a ete examine.
static void enregistrerPassage(int ligne, int colonne, int direction,
                               bool bloque) {
  int ligneVoisine;
  int colonneVoisine;
  trouverCaseVoisine(ligne, colonne, direction,
                     &ligneVoisine, &colonneVoisine);

  int bitExamine = bitDirectionExaminee(direction);
  int bitBloque = bitDirectionBloquee(direction);
  tableauParcours[ligne][colonne] |= bitExamine;
  tableauParcours[ligne][colonne] &= ~bitBloque;

  if (bloque) {
    tableauParcours[ligne][colonne] |= bitBloque;
  }

  // Une limite du tableau est connue, mais ne signifie pas qu'un mur a ete vu.
  if (ligneVoisine < 0 || ligneVoisine >= PARCOURS_LIGNES ||
      colonneVoisine < 0 || colonneVoisine >= PARCOURS_COLONNES) {
    return;
  }

  int directionOpposee = (direction + 2) % NOMBRE_DIRECTIONS;
  int bitExamineOppose = bitDirectionExaminee(directionOpposee);
  int bitBloqueOppose = bitDirectionBloquee(directionOpposee);
  tableauParcours[ligneVoisine][colonneVoisine] |= bitExamineOppose;
  tableauParcours[ligneVoisine][colonneVoisine] &=
      ~bitBloqueOppose;

  if (bloque) {
    tableauParcours[ligneVoisine][colonneVoisine] |= bitBloqueOppose;
  }
}

// Tourne le robot vers une direction et met a jour son orientation connue.
static void orienterVers(int direction) {
  int quartDeTourDroite =
      (direction + NOMBRE_DIRECTIONS - orientationActuelle) %
      NOMBRE_DIRECTIONS;

  if (quartDeTourDroite == 1) {
    tourne(90);
  } else if (quartDeTourDroite == 2) {
    tourne(180);
  } else if (quartDeTourDroite == 3) {
    tourne(-90);
  }

  orientationActuelle = direction;
}

// Met a jour la case du robot sans effacer les informations deja memorisees.
static void mettreAJourPosition(int nouvelleLigne, int nouvelleColonne) {
  tableauParcours[ligneActuelle][colonneActuelle] &=
      ~PARCOURS_COURANTE;
  ligneActuelle = nouvelleLigne;
  colonneActuelle = nouvelleColonne;
  tableauParcours[ligneActuelle][colonneActuelle] |=
      PARCOURS_VISITEE | PARCOURS_COURANTE;
}

// Termine la recherche des que le robot entre dans la rangee du haut.
static void verifierSortie() {
  if (ligneActuelle == 0) {
    arret();
    parcoursEstTermine = true;
    sortieAtteinte = true;
  }
}

// Immobilise le robot si la pile est vide sans que la sortie ait ete trouvee.
static void terminerSansSortie() {
  arret();
  parcoursEstTermine = true;
  sortieAtteinte = false;
}

// Ajoute une case a la pile et memorise par ou le robot devra revenir.
static void empilerCase(int ligne, int colonne, int directionRetour) {
  ++sommetPile;
  pileIndicesCases[sommetPile] = ligne * PARCOURS_COLONNES + colonne;
  pileDirectionsRetour[sommetPile] = directionRetour;
  pileProchainesDirections[sommetPile] = NORD;
}

// Reinitialise la carte et place le robot au depart, au centre de la rangee du
// bas, oriente vers le haut du parcours.
void initialiserParcours() {
  for (int ligne = 0; ligne < PARCOURS_LIGNES; ++ligne) {
    for (int colonne = 0; colonne < PARCOURS_COLONNES; ++colonne) {
      tableauParcours[ligne][colonne] = 0;
    }
  }

  ligneActuelle = PARCOURS_LIGNES - 1;
  colonneActuelle = PARCOURS_COLONNES / 2;
  orientationActuelle = NORD;
  sommetPile = -1;
  parcoursEstTermine = false;
  sortieAtteinte = false;
  parcoursInitialise = true;

  tableauParcours[ligneActuelle][colonneActuelle] =
      PARCOURS_VISITEE | PARCOURS_COURANTE;
  empilerCase(ligneActuelle, colonneActuelle, SUD);
}

// Explore une nouvelle branche ou revient d'une case deja exploree. Chaque
// appel fait au maximum un deplacement complet entre deux cases.
void parcourirUneEtape() {
  if (!parcoursInitialise) {
    initialiserParcours();
  }

  if (parcoursEstTermine) {
    arret();
    return;
  }

  verifierSortie();
  if (parcoursEstTermine) {
    return;
  }

  while (sommetPile >= 0) {
    int ligneCase =
        pileIndicesCases[sommetPile] / PARCOURS_COLONNES;
    int colonneCase =
        pileIndicesCases[sommetPile] % PARCOURS_COLONNES;

    // Cherche une direction non examinee dans la case courante de la pile.
    if (pileProchainesDirections[sommetPile] < NOMBRE_DIRECTIONS) {
      int direction = pileProchainesDirections[sommetPile]++;

      if ((tableauParcours[ligneCase][colonneCase] &
           bitDirectionExaminee(direction)) != 0) {
        continue;
      }

      int ligneVoisine;
      int colonneVoisine;
      trouverCaseVoisine(ligneCase, colonneCase, direction,
                         &ligneVoisine, &colonneVoisine);

      // Les bords de la grille ne sont pas des passages a sonder.
      if (ligneVoisine < 0 || ligneVoisine >= PARCOURS_LIGNES ||
          colonneVoisine < 0 || colonneVoisine >= PARCOURS_COLONNES) {
        enregistrerPassage(ligneCase, colonneCase, direction, false);
        continue;
      }

      // Le DFS n'a besoin que de visiter chaque case une fois pour trouver
      // une sortie; il ignore donc les liens vers les cases deja visitees.
      if ((tableauParcours[ligneVoisine][colonneVoisine] &
           PARCOURS_VISITEE) != 0) {
        continue;
      }

      orienterVers(direction);
      bool passageOuvert = mouvementAvant(DISTANCE_ENTRE_CASES_CM);
      enregistrerPassage(ligneCase, colonneCase, direction, !passageOuvert);

      if (!passageOuvert) {
        continue;
      }

      mettreAJourPosition(ligneVoisine, colonneVoisine);
      empilerCase(ligneActuelle, colonneActuelle,
                  (direction + 2) % NOMBRE_DIRECTIONS);
      verifierSortie();
      return;
    }

    // Si la case de depart est epuisee, aucune sortie n'a ete trouvee.
    if (sommetPile == 0) {
      terminerSansSortie();
      return;
    }

    // La branche est terminee : recule d'une case vers le parent du DFS.
    int directionRetour = pileDirectionsRetour[sommetPile];
    int ligneParent;
    int colonneParent;
    trouverCaseVoisine(ligneActuelle, colonneActuelle, directionRetour,
                       &ligneParent, &colonneParent);
    orienterVers(directionRetour);

    if (!mouvementAvant(DISTANCE_ENTRE_CASES_CM)) {
      // Le passage de retour n'est plus libre; la position reelle est incertaine.
      terminerSansSortie();
      return;
    }

    --sommetPile;
    mettreAJourPosition(ligneParent, colonneParent);
    return;
  }

  terminerSansSortie();
}

// Indique si l'exploration a atteint la sortie ou epuise ses branches.
bool parcoursTermine() {
  return parcoursEstTermine;
}

// Indique si le robot a atteint une case de la rangee du haut.
bool parcoursReussi() {
  return sortieAtteinte;
}
