#include <LibRobus.h>
#include "Son.h"
#include <Arduino.h>

//Dure de la confirmation pour que le robot ne demare pas avec un son random
const long long Duree_Confirmation_ms = 50;






// Joue plusieurs bips courts.
// Cette fonction bloque temporairement le programme pendant la sequence.
void beep(int count) {
  for (int i = 0; i < count; i++) {
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);
  }

  // Pause pour separer cette iteration pour pas spam les beep.
  delay(400);
}




// Initialise le module de son si necessaire. Pour le moment, il n'y a rien a initialiser.
void initialiserSon() {
  // Rien a initialiser pour le moment.
  //pinMode(PIN_SON, INPUT); A TROUVER
  pinMode(PIN_SON, INPUT);
}

bool detecter5kHz() {
  int lecture = analogRead(PIN_SON);

  if (lecture > Seuil_Tension) {
    unsigned long startTime = millis();
    while (millis() - startTime < Duree_Confirmation_ms) {

      if (analogRead(PIN_SON) <= Seuil_Tension) {
        return false; // Le signal est tombé en dessous du seuil, donc ce n'est pas un 5kHz.
      }
      delay(2); // Pause pour pas run le code trop vite
    }
    return true; // Le signal a été détecté pendant la durée de confirmation.
  }
  return false; // Le signal n'a pas été détecté.
}