
//librairies 
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include "traitement_couleur.h"
#include <LibRobus.h>


// Initialisation du capteur
Adafruit_TCS34725 capteur = Adafruit_TCS34725(
    TCS34725_INTEGRATIONTIME_50MS, 
    TCS34725_GAIN_4X
);

bool initialiserCapteurCouleur() {
    return capteur.begin();
}

// ---------------------------------------------------------------
// tableau d'échantillons de couleurs
// ---------------------------------------------------------------

static Couleur echantillons[NB_ECHANTILLONS];
static int indexEchantillon = 0;
static int nbRecues = 0;

// ---------------------------------------------------------------
// LECTURE DU CAPTEUR (dépend du capteur utilisé, à compléter)
// ---------------------------------------------------------------
Couleur lireCapteur() {
    Couleur c;

    uint16_t rouge, vert, bleu, clair;  //j'utilise pas le valeur de clair pour l'instant,
                                        // mais on peut l'utiliser pour la luminosité si besoin

    capteur.getRawData(&rouge, &vert, &bleu, &clair);

    c.R = rouge;
    c.G = vert;
    c.B = bleu;

    return c;
}
//---------------------------------------------------------------
// MÉMOIRE DES ÉCHANTILLONS
// ---------------------------------------------------------------
// Ajoute une lecture à la mémoire (écrase la plus ancienne quand c'est plein)

void ajouterEchantillon(Couleur c) {
    echantillons[indexEchantillon] = c;
    indexEchantillon = (indexEchantillon + 1) % NB_ECHANTILLONS;
    if (nbRecues < NB_ECHANTILLONS) {
        nbRecues++;
    }
}

// Remet la mémoire à zéro (à appeler avant une nouvelle série de mesures)
void viderEchantillons() {
    indexEchantillon = 0;
    nbRecues = 0;
}

// ---------------------------------------------------------------
// CALCUL DE LA COULEUR MOYENNE
//Trouve la couleur moyenne capter après 10 échantillons 
// ---------------------------------------------------------------

Couleur moyenneCouleur() {
    Couleur somme = {0, 0, 0};
    for (int i = 0; i < nbRecues; i++) {
        somme.R += echantillons[i].R;
        somme.G += echantillons[i].G;
        somme.B += echantillons[i].B;
    }
    if (nbRecues > 0) {
        somme.R /= nbRecues;
        somme.G /= nbRecues;
        somme.B /= nbRecues;
    }
    return somme;
}

// ---------------------------------------------------------------
// IDENTIFICATION DE LA COULEUR
//plage de valeurs pour chaque couleur
// ---------------------------------------------------------------

CouleurId identifierCouleur(Couleur m) {
  // ---------------------------------------------------------
  // SEUILS (à remplir selon les mesures réelles)
   if (196<m.R && m.R<211 && 161<m.G && m.G<173 && 142<m.B && m.B<153) return ROSE;
   if (156<m.R && m.R<176 && 184<m.G && m.G<205 && 116<m.B && m.B<128) return JAUNE;
   if (130<m.R && m.R<159 && 128<m.G && m.G<153 && 94<m.B && m.B<109) return ORANGE;
   if (80<m.B && m.B<93 && 131<m.R && m.R<156 && 126<m.G && m.G<152) return BLEU;
   if (109<m.G && m.G<121 && 162<m.R && m.R<177 && 137<m.B && m.B<150) return VERT;
  // ---------------------------------------------------------
  return AUCUN;
}

// ---------------------------------------------------------------
// DÉBOGAGE
//change le numéro de couleur en texte pour affichage (ex: 1 -> "ROSE"), pour vérifier le fonctionnement du code
// ---------------------------------------------------------------
String nomCouleur(CouleurId id) {
  switch (id) {
    case ROSE:   return "Rose";
    case JAUNE:  return "Jaune";
    case ORANGE: return "Orange";
    case BLEU:   return "Bleu";
    case VERT:   return "Vert";
    default:     return "Aucune";
  }
}
// ---------------------------------------------------------------
// CALIBRATION DE LA COULEUR
//utiliser pour trouver les seuils de couleur seulement, ne pas utiliser dans le programme final
// ---------------------------------------------------------------

void calibrerCouleur(const char* nom, int duree_ms) {   
    // Variables pour le calcul des min, max et moyenne
    float minR = 1e9,  minG = 1e9,  minB = 1e9;         // minimums 
    float maxR = 0, maxG =0, maxB = 0;                  // maximums 
    float sommeR = 0, sommeG = 0, sommeB = 0;           // pour la moyenne
    int n = 0;                                          // nombre de lectures prises

    // Boucle de lecture pendant la durée spécifiée
    unsigned long debut = millis();                     
    while (millis() - debut < (unsigned long)duree_ms) {  // boucle sur la durée voulue
        Couleur c = lireCapteur();                      

        if (c.R < minR) minR = c.R;                     // Trouver min RGB
        if (c.G < minG) minG = c.G;
        if (c.B < minB) minB = c.B;
        if (c.R > maxR) maxR = c.R;                     // Trouver max RGB
        if (c.G > maxG) maxG = c.G;
        if (c.B > maxB) maxB = c.B;

        sommeR += c.R;                                 //moyenne RGB
        sommeG += c.G;
        sommeB += c.B;
        n++;                                           
        delay(50);                                     
    }

    if (n == 0) return;                                 // aucune lecture : on sort

    Serial.print(nom);                                  // affichage des résultats
    Serial.print(" | R: "); Serial.print(minR);
    Serial.print(" a ");    Serial.print(maxR);
    Serial.print(" (moy "); Serial.print(sommeR / n); Serial.print(")");

    Serial.print(" | G: "); Serial.print(minG);
    Serial.print(" a ");    Serial.print(maxG);
    Serial.print(" (moy "); Serial.print(sommeG / n); Serial.print(")");

    Serial.print(" | B: "); Serial.print(minB);
    Serial.print(" a ");    Serial.print(maxB);
    Serial.print(" (moy "); Serial.print(sommeB / n); Serial.println(")");
}

void modeCalibration() {
  const char* noms[] = {"Rose", "Jaune", "Orange", "Bleu", "Vert"};
  for (int i = 0; i < 5; i++) {                           //boucle sur les couleurs à calibrer
    Serial.print("Place le capteur sur : ");
    Serial.println(noms[i]);
    while (!ROBUS_IsBumper(2)) {}   // attend l'appui sur le bumper avant
    delay(500);
    calibrerCouleur(noms[i], 3000); // lit pendant 3 secondes
  }
}
