/*
Projet: Main - P13
Auteurs: Alexandre H., Luis
Description: Code pour sortie du labyrinthe
Date : 01/10/2026
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
#include <avr/interrupt.h>


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
const int LED_ROSE = 32;
const int LED_JAUNE = 33;
const int LED_ORANGE = 38;
const int LED_BLEU = 39;
const int LED_VERT = 40;
const int LECTURES_COULEUR_REQUISES = 8;
volatile int ledChronometree = -1;
volatile uint16_t ticksLed = 0;
bool capteurCouleurPret = false;

// Timer3 overflow coupe la LED à 10 secondes même pendant une épreuve
// bloquante. En PWM phase-correct AVR, un overflow dure environ 2,04 ms.
ISR(TIMER3_OVF_vect) {
  if (ledChronometree >= 0 && ++ticksLed >= 4902) {
    digitalWrite(ledChronometree, LOW);
    ledChronometree = -1;
    TIMSK3 &= ~_BV(TOIE3);
  }
}

void allumerLedDixSecondes(int pin) {
  noInterrupts();
  if (ledChronometree >= 0) digitalWrite(ledChronometree, LOW);
  digitalWrite(pin, HIGH);
  ledChronometree = pin;
  ticksLed = 0;
  TCNT3 = 0;
  TIFR3 = _BV(TOV3);
  TIMSK3 |= _BV(TOIE3);
  interrupts();
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
  initialiserDetecteurProximite();
  initialiserSuiveurLigne();
  Serial.begin(115200);
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  delay(1000);
  beep(1);
  initialiserSon();
  capteurCouleurPret = initialiserCapteurCouleur();
  if (capteurCouleurPret) {
    Serial.println("Capteur couleur TCS34725 detecte; LED d'eclairage activee.");
  } else {
    Serial.println("TCS34725 absent: verifier VIN, GND, SDA et SCL.");
  }

  // Les bumpers sont lus dans loop(); on commence sans mouvement.
  arret();
}

void loop() {

  CouleurId couleurLue = AUCUN;
  if (capteurCouleurPret) {
    Couleur mesure = lireCapteur();
    couleurLue = identifierCouleur(mesure);
    static unsigned long dernierAffichageCouleur = 0;
    if (millis() - dernierAffichageCouleur >= 500) {
      dernierAffichageCouleur = millis();
      Serial.print("RGB brut: ");
      Serial.print(mesure.R); Serial.print('/');
      Serial.print(mesure.G); Serial.print('/');
      Serial.print(mesure.B); Serial.print(" -> ");
      Serial.println(nomCouleur(couleurLue));
    }
  }
  if (couleurLue == couleurCandidate) { 
    if (lecturesCouleurStables < LECTURES_COULEUR_REQUISES)
      lecturesCouleurStables++;
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
  continuer();
  delay(10);

  bool Arr = ROBUS_IsBumper(3);
  if (Arr) {
    delay(500);
    mouvementAvant(150.0f);
    arret();
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

