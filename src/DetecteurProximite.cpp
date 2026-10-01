#include <DetecteurProximite.h>
#include <Mouvement.h>

// Pins des deux capteurs et dernieres valeurs lues.
const int vertpin = 49;
const int rougepin = 48;
bool ObstacleDetecte = false;
bool vert = false;
bool rouge = false;

// Configure les pins en entree. Cette fonction est appelee une fois dans setup().
void initialiserDetecteurProximite() {
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
}

// Met a jour Vert et Rouge et renvoie si besoin d'arret
void detecterObstacle() {

  vert = digitalRead(vertpin);
  rouge = digitalRead(rougepin);

  if (!(vert && rouge) or !vert or !rouge) {
    ObstacleDetecte = true;
    Serial.println("Obstacle detecte");
  }  
  
  
}
