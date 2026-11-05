
//librairies 
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include "traitement_couleur.h"
#include <LibRobus.h>
#include <string.h>


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

struct PlageCouleur {
    CouleurId id;
    const char* nom;
    Couleur minimum;
    Couleur maximum;
};

// Valeurs initiales, remplacées par les mesures lors de la calibration.
static PlageCouleur plages[] = {
    {ROSE, "Rose", {312, 288, 181}, {409, 384, 233}},
    {JAUNE, "Jaune", {340, 422, 228}, {413, 517, 279}},
    {ORANGE, "Orange", {369, 29, 265}, {469, 378, 332}},
    {BLEU, "Bleu", {350, 155, 145}, {481, 398, 491}},
    {VERT, "Vert", {350, 155, 145}, {481, 398, 491}}
};
// {350, 155, 145}, {481, 398, 491}} vert
CouleurId identifierCouleur(Couleur m) {
    const float luminosite = m.R + m.G + m.B;
    if (luminosite <= 0) return AUCUN;

    CouleurId resultat = AUCUN;
    float meilleureDistance = 1e9f;
    float deuxiemeDistance = 1e9f;
    for (const auto &plage : plages) {
        // Moyennes des plages existantes, sans nouvelle calibration.
        const float rouge = (plage.minimum.R + plage.maximum.R) * 0.5f;
        const float vert = (plage.minimum.G + plage.maximum.G) * 0.5f;
        const float bleu = (plage.minimum.B + plage.maximum.B) * 0.5f;
        const float total = rouge + vert + bleu;
        if (total <= 0 || luminosite < total * 0.35f ||
            luminosite > total * 2.5f) continue;

        // Les proportions changent moins que les valeurs brutes avec l'éclairage.
        const float ecartR = m.R / luminosite - rouge / total;
        const float ecartG = m.G / luminosite - vert / total;
        const float ecartB = m.B / luminosite - bleu / total;
        const float distance = ecartR * ecartR + ecartG * ecartG + ecartB * ecartB;
        if (distance < meilleureDistance) {
            deuxiemeDistance = meilleureDistance;
            meilleureDistance = distance;
            resultat = plage.id;
        } else if (distance < deuxiemeDistance) {
            deuxiemeDistance = distance;
        }
    }

    // Rejeter les couleurs éloignées et les choix trop ambigus.
    if (meilleureDistance > 0.012f ||
        meilleureDistance >= deuxiemeDistance * 0.8f) return AUCUN;
    return resultat;
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
// Mesurer et appliquer les plages en mémoire pour la session courante.
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

    // Marge de 5 % autour des mesures, avec au moins deux unités ADC.
    const float margeR = max(2.0f, (sommeR / n) * 0.05f);
    const float margeG = max(2.0f, (sommeG / n) * 0.05f);
    const float margeB = max(2.0f, (sommeB / n) * 0.05f);
    for (auto &plage : plages) {
        if (strcmp(plage.nom, nom) == 0) {
            plage.minimum = {
                max(0.0f, minR - margeR),
                max(0.0f, minG - margeG),
                max(0.0f, minB - margeB)
            };
            plage.maximum = {maxR + margeR, maxG + margeG, maxB + margeB};
        }
    }
    Serial.println("Plage de detection appliquee pour cette session.");

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
    while (ROBUS_IsBumper(2)) { delay(10); } // Relâcher avant chaque couleur.
    while (!ROBUS_IsBumper(2)) { delay(10); }   // attend l'appui sur le bumper avant
    delay(500);
    calibrerCouleur(noms[i], 3000); // lit pendant 3 secondes
  }
}
