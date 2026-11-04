#include <pince.h>


//initialisation du servo de la pince
void initialiserPince() {
    pince.attach(pinServo);
    pince.write(angleOuverture); // Position initiale de la pince (ouverte)
}


//mouvement d'ouverture/fermeture de la pince
void pinceOuverture() {
    pince.write(angleOuverture);
}

void pinceFermeture() {
    pince.write(angleFermeture);
}
