/*
Projet: Main - P13
Auteurs: Alexandre H., Luis
Description: Code pour sortie du labyrinthe
Date : 01/10/2026


Rose | R: 60.00 a 61.00 (moy 60.41) | G: 20.00 a 21.00 (moy 20.97) | B: 35.00 a 35.00 (moy 35.00)
Place le capteur sur : Jaune
Jaune | R: 74.00 a 75.00 (moy 74.45) | G: 50.00 a 51.00 (moy 50.21) | B: 24.00 a 24.00 (moy 24.00)
Place le capteur sur : Orange
Orange | R: 56.00 a 57.00 (moy 56.97) | G: 12.00 a 13.00 (moy 12.97) | B: 9.00 a 9.00 (moy 9.00)
Place le capteur sur : Bleu
Bleu | R: 8.00 a 8.00 (moy 8.00) | G: 26.00 a 27.00 (moy 26.10) | B: 50.00 a 51.00 (moy 50.34)
Place le capteur sur : Vert
Vert | R: 6.00 a 6.00 (moy 6.00) | G: 13.00 a 13.00 (moy 13.00) | B: 5.00 a 6.00 (moy 5.03)


*/

/*
Inclure les librairies de functions que vous voulez utiliser
*/

#include <LibRobus.h>
#include <Mouvement.h>
#include <DetecteurProximite.h>
#include <Parcours.h>
#include "Son.h"
#include <EtapeCombattant.h>
#include <traitement_couleur.h>
#include <DetecteurSuiveurLigne.h>
#include <pince.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


/*
Variables globales et defines
*/
// L'exploration ne commence qu'apres le signal sonore de depart.
bool robotDemarre = false;
bool bumperGa; //0
bool bumperDr; //1
bool bumperAv; //2
bool bumperArr; //3

int etat = 0; // = 0=Arret 1=Avance 2=Droite 3=Gauche 4=Recule 
int etatPast = 0;

// D22-D25 sont réservées au sonar, D26-D29 aux bumpers,
// D30-D31 aux moteurs, D34-D35 aux encodeurs et D36 au buzzer.
// Les cinq LED utilisent des broches libres.
const int LED_ROSE = 38;
const int LED_JAUNE = 39;
const int LED_ORANGE = 40;
const int LED_BLEU = 41;
const int LED_VERT = 42;
const uint8_t AUCUNE_LED = 0xFF;
const uint16_t DUREE_DEL_TICKS = 9766;
const int LECTURES_COULEUR_REQUISES = 4;
volatile uint8_t ledChronometree = AUCUNE_LED;
volatile uint16_t ticksRestantsDEL = 0;

ISR(TIMER0_COMPA_vect) {
  if (ticksRestantsDEL > 0 && --ticksRestantsDEL == 0) {
    digitalWrite(ledChronometree, LOW);
    ledChronometree = AUCUNE_LED;
  }
}

void allumerLedDixSecondes(int pin) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (ledChronometree != AUCUNE_LED) {
      digitalWrite(ledChronometree, LOW);
    }
    ledChronometree = pin;
    ticksRestantsDEL = DUREE_DEL_TICKS;
    digitalWrite(ledChronometree, HIGH);
  }
}
// Confirmation des lectures pour eviter plusieurs sequences de bips
// lorsque le capteur oscille entre deux couleurs.
CouleurId couleurCandidate = AUCUN;
CouleurId derniereCouleurAnnoncee = AUCUN;
int lecturesCouleurStables = 0;

/*
Fonctions d'initialisation (setup)
*/
void setup(){
  BoardInit();
  pinMode(LED_ROSE, OUTPUT);
  pinMode(LED_JAUNE, OUTPUT);
  pinMode(LED_ORANGE, OUTPUT);
  pinMode(LED_BLEU, OUTPUT);
  pinMode(LED_VERT, OUTPUT);
  digitalWrite(LED_ROSE, LOW);
  digitalWrite(LED_JAUNE, LOW);
  digitalWrite(LED_ORANGE, LOW);
  digitalWrite(LED_BLEU, LOW);
  digitalWrite(LED_VERT, LOW);
  // Timer0 compare A runs every 1.024 ms, independently of blocking routines.
  OCR0A = 128;
  TIFR0 = _BV(OCF0A);
  TIMSK0 |= _BV(OCIE0A);
  initialiserDetecteurProximite();
  initialiserSuiveurLigne();
  Serial.begin(115200);
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  delay(1000);
  initialiserPince();
  initialiserSon();
  initialiserCapteurCouleur();

  // Le robot reste immobile en attendant le message IR de démarrage.
  arret();
}

void traiterCouleurDuParcours() {
  CouleurId couleurLue = identifierCouleur(lireCapteur());
  if (couleurLue == couleurCandidate) { 
    if (lecturesCouleurStables < LECTURES_COULEUR_REQUISES) lecturesCouleurStables++;
  } else {
    couleurCandidate = couleurLue;
    lecturesCouleurStables = 1;
  }

  if (lecturesCouleurStables >= LECTURES_COULEUR_REQUISES &&
      couleurCandidate == AUCUN) {
    derniereCouleurAnnoncee = AUCUN;
  }

  if (lecturesCouleurStables >= LECTURES_COULEUR_REQUISES &&
      couleurCandidate != AUCUN && couleurCandidate != derniereCouleurAnnoncee) {
    int nombreBips = 0;
    int led = -1;
    switch (couleurCandidate) {
      case ROSE:   nombreBips = 1; led = LED_ROSE; break;
      case JAUNE:  nombreBips = 2; led = LED_JAUNE; break;
      case ORANGE: nombreBips = 3; led = LED_ORANGE; break;
      case BLEU:   nombreBips = 4; led = LED_BLEU; break;
      case VERT:   nombreBips = 5; led = LED_VERT; break;
      default: break;
    }

    if (nombreBips > 0) {
      arret();
      allumerLedDixSecondes(led);
      beep(nombreBips);
      switch (couleurCandidate) {
        case ROSE:   EtapeRouge(); break; 
        case JAUNE:  EtapeJaune(); break;
        case ORANGE: EtapeOrange(); break;
        case BLEU:   EtapeBleu(); break;
        case VERT:   EtapeVert(); break;
        default: break;
      }

    }
    derniereCouleurAnnoncee = couleurCandidate;
  }
}

void loop() {
  static bool parcoursActif = false;
  const uint32_t CODE_DEMARRAGE = 0x08F7807FUL;

  // Lire une seule fois le message reçu pendant ce passage dans loop().
  const uint32_t message = REMOTE_read();

  if (!parcoursActif) {
    arret();
    // 0 indique l'absence de message et FFFFFFFF une répétition NEC.
    if (message == 0 || message == 0xFFFFFFFFUL) {
      return;
    }
    if (message != CODE_DEMARRAGE) {
      return;
    }

    beep(2); // Confirme le démarrage avant de commander les moteurs.
    reinitialiserSuiveurLigne();
    parcoursActif = true;
  }

  if (parcoursActif) {
    traiterCouleurDuParcours();
    suivreLigne();
    return;
  }

  bool Gauche = ROBUS_IsBumper(0);
  if (Gauche) {
    delay(1000);
    tourne(-90);
  }
  // Bumper 1 : lance la calibration des couleurs
  bool droite = ROBUS_IsBumper(1);
  if (droite) {
    delay(500);          
    modeCalibration();
  }
  bool Av = ROBUS_IsBumper(2);
    if (Av) {
      delay(500);
      EtapeRouge();
    }
}
