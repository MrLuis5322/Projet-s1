#include <DetecteurSuiveurLigne.h>

// Pins des deux capteurs et dernieres valeurs lues.
const int gauche = A0; // U1
const int centre = A1; // U2
const int droite = A2; // U3
const int SEUIL_LIGNE_ADC = 512; // seuil provisoire à ajuster avec les valeurs Serial

// Configure les pins en entree. Cette fonction est appelee une fois dans setup().
void initialiserSuiveurLigne() {
  pinMode(gauche, INPUT);
  pinMode(centre, INPUT);
  pinMode(droite, INPUT);
}

int detecterPositionRobot() {
    int valeurG = analogRead(gauche);
    int valeurC = analogRead(centre);
    int valeurD = analogRead(droite);
    bool g = valeurG > SEUIL_LIGNE_ADC;
    bool c = valeurC > SEUIL_LIGNE_ADC;
    bool d = valeurD > SEUIL_LIGNE_ADC;

    // Affiche les valeurs brutes deux fois par seconde pour permettre de
    // régler SEUIL_LIGNE_ADC selon le capteur et la surface réels.
    static unsigned long dernierAffichage = 0;
    if (millis() - dernierAffichage >= 500) {
        dernierAffichage = millis();
        Serial.print("Ligne A0/A1/A2: ");
        Serial.print(valeurG); Serial.print('/');
        Serial.print(valeurC); Serial.print('/');
        Serial.println(valeurD);
    }

    if      (!g && !c && !d)  return 0; // 000 : aucun capteur sur la ligne (ligne perdue)
    else if (!g && !c &&  d)  return 1; // 001 : ligne à droite
    else if (!g &&  c && !d)  return 2; // 010 : ligne au centre, tout droit
    else if (!g &&  c &&  d)  return 3; // 011 : ligne centre-droite
    else if ( g && !c && !d)  return 4; // 100 : ligne à gauche
    else if ( g && !c &&  d)  return 5; // 101 : cas bizarre (ligne large ou intersection)
    else if ( g &&  c && !d)  return 6; // 110 : ligne centre-gauche
    else                      return 7; // 111 : tout noir (intersection ou fin de ligne)
}
