#include <Arduino.h>
#include <Mouvement.h>
#include "Parcours.h"

// Chaque case mesure 50 cm dans le plan du parcours.
static const float LONGUEUR_CASE_CM = 50.0f;
static const uint8_t NOMBRE_DIRECTIONS = 4;
static const uint8_t CAPACITE_PILE =
    PARCOURS_LIGNES * PARCOURS_COLONNES;

enum Direction {
  NORD = 0,
  EST = 1,
  SUD = 2,
  OUEST = 3
};

// Ces tableaux forment une pile DFS sans objets : ils memorisent les cases
// a revisiter, la direction de retour et les directions restant a explorer.
static uint8_t pileLigne[CAPACITE_PILE];
static uint8_t pileColonne[CAPACITE_PILE];
static uint8_t pileDirectionRetour[CAPACITE_PILE];
static uint8_t pileProchaineDirection[CAPACITE_PILE];
static int sommetPile = -1;

// Position et orientation courantes du robot dans le repere de la grille.
static uint8_t ligneRobot = PARCOURS_LIGNES - 1;
static uint8_t colonneRobot = PARCOURS_COLONNES / 2;
static uint8_t orientationRobot = NORD;
static bool estInitialise = false;
static bool estTermine = false;
static bool estReussi = false;

uint16_t tableauParcours[PARCOURS_LIGNES][PARCOURS_COLONNES];

// Associe une direction a sa variation de ligne et de colonne.
static void obtenirVoisin(uint8_t ligne, uint8_t colonne, uint8_t direction,
                          int8_t *ligneVoisine, int8_t *colonneVoisine) {
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

// Retourne les indicateurs de direction correspondants dans la carte 10 x 3.
static uint16_t indicateurTeste(uint8_t direction) {
  return (uint16_t)(PARCOURS_TESTE_NORD << direction);
}

static uint16_t indicateurBloque(uint8_t direction) {
  return (uint16_t)(PARCOURS_BLOQUE_NORD << direction);
}

// Memorise l'etat d'un passage des deux cotes de la frontiere entre les cases.
// Un passage ouvert est teste sans recevoir le bit BLOQUE.
static void enregistrerPassage(uint8_t ligne, uint8_t colonne,
                               uint8_t direction, bool bloque) {
  int8_t ligneVoisine;
  int8_t colonneVoisine;
  obtenirVoisin(ligne, colonne, direction, &ligneVoisine, &colonneVoisine);

  uint16_t teste = indicateurTeste(direction);
  uint16_t bloqueBit = indicateurBloque(direction);
  tableauParcours[ligne][colonne] |= teste;
  tableauParcours[ligne][colonne] &= (uint16_t)~bloqueBit;
  if (bloque) {
    tableauParcours[ligne][colonne] |= bloqueBit;
  }

  if (ligneVoisine < 0 || ligneVoisine >= PARCOURS_LIGNES ||
      colonneVoisine < 0 || colonneVoisine >= PARCOURS_COLONNES) {
    return;
  }

  uint8_t directionOpposee = (uint8_t)((direction + 2) % NOMBRE_DIRECTIONS);
  uint16_t testeOppose = indicateurTeste(directionOpposee);
  uint16_t bloqueOppose = indicateurBloque(directionOpposee);
  tableauParcours[ligneVoisine][colonneVoisine] |= testeOppose;
  tableauParcours[ligneVoisine][colonneVoisine] &=
      (uint16_t)~bloqueOppose;
  if (bloque) {
    tableauParcours[ligneVoisine][colonneVoisine] |= bloqueOppose;
  }
}

// Oriente le robot vers un point cardinal et garde la carte d'orientation
// coherente avec les rotations executees par les encodeurs.
static void orienterVers(uint8_t direction) {
  uint8_t rotationDroite =
      (uint8_t)((direction + NOMBRE_DIRECTIONS - orientationRobot) %
                NOMBRE_DIRECTIONS);

  if (rotationDroite == 1) {
    tourne(90);
  } else if (rotationDroite == 2) {
    tourne(180);
  } else if (rotationDroite == 3) {
    tourne(-90);
  }

  orientationRobot = direction;
}

// Met a jour l'indicateur de case courante sans effacer l'historique des cases
// deja visitees.
static void definirPosition(uint8_t nouvelleLigne, uint8_t nouvelleColonne) {
  tableauParcours[ligneRobot][colonneRobot] &= (uint16_t)~PARCOURS_COURANTE;
  ligneRobot = nouvelleLigne;
  colonneRobot = nouvelleColonne;
  tableauParcours[ligneRobot][colonneRobot] |=
      PARCOURS_VISITEE | PARCOURS_COURANTE;
}

// Arrete la recherche quand une case de la rangee superieure est atteinte.
static void verifierArrivee() {
  if (ligneRobot == 0) {
    arret();
    estTermine = true;
    estReussi = true;
  }
}

// Marque la recherche comme impossible et immobilise le robot.
static void signalerEchec() {
  arret();
  estTermine = true;
  estReussi = false;
}

// Cree la premiere entree de la pile de recherche a la position de depart.
static void empilerCase(uint8_t ligne, uint8_t colonne,
                        uint8_t directionRetour) {
  if (sommetPile + 1 >= CAPACITE_PILE) {
    signalerEchec();
    return;
  }

  ++sommetPile;
  pileLigne[sommetPile] = ligne;
  pileColonne[sommetPile] = colonne;
  pileDirectionRetour[sommetPile] = directionRetour;
  pileProchaineDirection[sommetPile] = NORD;
}

// Efface la carte et prepare la recherche depuis le depart central du bas.
void initialiserParcours() {
  for (uint8_t ligne = 0; ligne < PARCOURS_LIGNES; ++ligne) {
    for (uint8_t colonne = 0; colonne < PARCOURS_COLONNES; ++colonne) {
      tableauParcours[ligne][colonne] = 0;
    }
  }

  ligneRobot = PARCOURS_LIGNES - 1;
  colonneRobot = PARCOURS_COLONNES / 2;
  orientationRobot = NORD;
  sommetPile = -1;
  estTermine = false;
  estReussi = false;
  estInitialise = true;

  tableauParcours[ligneRobot][colonneRobot] =
      PARCOURS_VISITEE | PARCOURS_COURANTE;
  empilerCase(ligneRobot, colonneRobot, SUD);
}

// Teste les passages inconnus avec le capteur avant. Si un passage est libre,
// le robot avance d'une case et cette nouvelle case devient le sommet du DFS.
// Quand une case n'a plus de direction a explorer, le robot revient a son
// parent par un passage deja emprunte.
void parcourirUneEtape() {
  if (!estInitialise) {
    initialiserParcours();
  }
  if (estTermine) {
    arret();
    return;
  }

  verifierArrivee();
  if (estTermine) {
    return;
  }

  while (sommetPile >= 0) {
    uint8_t ligne = pileLigne[sommetPile];
    uint8_t colonne = pileColonne[sommetPile];

    if (pileProchaineDirection[sommetPile] < NOMBRE_DIRECTIONS) {
      uint8_t direction = pileProchaineDirection[sommetPile]++;
      uint16_t teste = indicateurTeste(direction);
      if ((tableauParcours[ligne][colonne] & teste) != 0) {
        continue;
      }

      int8_t ligneVoisine;
      int8_t colonneVoisine;
      obtenirVoisin(ligne, colonne, direction,
                    &ligneVoisine, &colonneVoisine);

      // Les limites de la grille sont des frontieres connues, pas des murs a
      // sonder physiquement avec le robot.
      if (ligneVoisine < 0 || ligneVoisine >= PARCOURS_LIGNES ||
          colonneVoisine < 0 || colonneVoisine >= PARCOURS_COLONNES) {
        enregistrerPassage(ligne, colonne, direction, true);
        continue;
      }

      // Un chemin vers une case deja exploree n'ajoute rien au DFS; le passage
      // inverse du chemin de recherche est deja enregistre comme ouvert.
      if ((tableauParcours[ligneVoisine][colonneVoisine] &
           PARCOURS_VISITEE) != 0) {
        continue;
      }

      orienterVers(direction);
      bool passageOuvert = mouvementAvant(LONGUEUR_CASE_CM);
      enregistrerPassage(ligne, colonne, direction, !passageOuvert);

      if (!passageOuvert) {
        // mouvementAvant recule jusqu'au point de depart de la sonde.
        continue;
      }

      definirPosition((uint8_t)ligneVoisine, (uint8_t)colonneVoisine);
      empilerCase(ligneRobot, colonneRobot,
                  (uint8_t)((direction + 2) % NOMBRE_DIRECTIONS));
      verifierArrivee();
      return;
    }

    // Une branche est epuisee. Remonter vers sa case parente par le passage
    // inverse du deplacement qui a mene a cette branche.
    if (sommetPile == 0) {
      signalerEchec();
      return;
    }

    uint8_t directionRetour = pileDirectionRetour[sommetPile];
    uint8_t ancienneLigne = ligneRobot;
    uint8_t ancienneColonne = colonneRobot;
    int8_t ligneParente;
    int8_t colonneParente;
    obtenirVoisin(ligneRobot, colonneRobot, directionRetour,
                  &ligneParente, &colonneParente);
    orienterVers(directionRetour);

    if (!mouvementAvant(LONGUEUR_CASE_CM)) {
      // Un obstacle apparu sur un passage deja emprunte rend le retour
      // impossible avec le plan courant; on s'arrete plutot que de deviner.
      enregistrerPassage(ancienneLigne, ancienneColonne,
                         directionRetour, true);
      signalerEchec();
      return;
    }

    --sommetPile;
    definirPosition((uint8_t)ligneParente, (uint8_t)colonneParente);
    return;
  }

  signalerEchec();
}

// Retourne l'etat de fin de l'exploration pour que main.cpp puisse choisir
// d'attendre, de s'arreter ou de signaler la fin.
bool parcoursTermine() {
  return estTermine;
}

// Retourne vrai uniquement si le robot a atteint la rangee du haut.
bool parcoursReussi() {
  return estReussi;
}
