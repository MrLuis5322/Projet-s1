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

// Lit les deux sorties du detecteur avant. Une sortie LOW indique un obstacle.
void detecterObstacle() {
  vert = digitalRead(vertpin);
  rouge = digitalRead(rougepin);

  // Recalcule l'etat a chaque lecture afin qu'une ancienne detection ne reste
  // pas active apres que le robot se soit eloigne de l'obstacle.
  ObstacleDetecte = !(vert && rouge);
  if (ObstacleDetecte) {
    Serial.println("Obstacle detecte");
  }
}
