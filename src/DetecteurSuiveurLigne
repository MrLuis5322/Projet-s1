#include <DetecteurSuiveurLigne.h>

// Pins des deux capteurs et dernieres valeurs lues.
const int gauche = A0; // U1
const int centre = A1; // U2
const int droite = A2; // U3

// Configure les pins en entree. Cette fonction est appelee une fois dans setup().
void initialiserSuiveurLigne() {
  pinMode(gauche, INPUT);
  pinMode(centre, INPUT);
  pinMode(droite, INPUT);
}

int detecterPositionRobot() {

    int pins[3] = {gauche, centre, droite};
    bool lecturesCapteurs[3] = {false, false, false}; // True : Noir , False : Autre

    for (int i = 0; i < 3; i++){
        int valeur = analogRead(pins[i]); // 0 à 1023 (ADC 10 bits)
        float tension = valeur * 5.0 / 1023.0;
        if(tension > 3.5f) lecturesCapteurs[i] = true;
        else lecturesCapteurs[i] = false;
    }

    bool g = lecturesCapteurs[0];
    bool c = lecturesCapteurs[1];
    bool d = lecturesCapteurs[2];

    if      (!g && !c && !d)  return 0; // 000 : aucun capteur sur la ligne (ligne perdue)
    else if (!g && !c &&  d)  return 1; // 001 : ligne à droite
    else if (!g &&  c && !d)  return 2; // 010 : ligne au centre, tout droit
    else if (!g &&  c &&  d)  return 3; // 011 : ligne centre-droite
    else if ( g && !c && !d)  return 4; // 100 : ligne à gauche
    else if ( g && !c &&  d)  return 5; // 101 : cas bizarre (ligne large ou intersection)
    else if ( g &&  c && !d)  return 6; // 110 : ligne centre-gauche
    else                      return 7; // 111 : tout noir (intersection ou fin de ligne)
}
