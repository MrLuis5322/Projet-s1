#include <LibRobus.h>
#include "Son.h"
#include <Arduino.h>


/*
Fonction : Faire biper le micro du robot un certain nombre de fois.
Entrée : Nombre de beeps
Retour : Rien.
*/
void beep(int count)
{
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
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