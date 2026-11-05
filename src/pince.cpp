#include <Arduino.h>
#include <LibRobus.h>
#include <Mouvement.h>
#include <pince.h>
#include <math.h>

namespace {
const uint8_t ANGLE_OUVERTURE = 0;
const uint8_t ANGLE_FERMETURE = 90;
const float RAYON_CERCLE_CM = 50.0f;
const uint8_t NOMBRE_SEGMENTS_CERCLE = 12;
const float ANGLE_SEGMENT_DEGRES = 360.0f / NOMBRE_SEGMENTS_CERCLE;
}

void initialiserPince() {
    SERVO_SetAngle(SERVO_1, ANGLE_OUVERTURE);
}

void pinceOuverture() {
    SERVO_SetAngle(SERVO_1, ANGLE_OUVERTURE);
}

void pinceFermeture() {
    SERVO_SetAngle(SERVO_1, ANGLE_FERMETURE);
}

void prendreVerre() {
    const float longueurSegmentCM =
        2.0f * RAYON_CERCLE_CM * sinf(ANGLE_SEGMENT_DEGRES * PI / 360.0f);

    pinceOuverture();
    delay(500);
    tourne(180.0f);

    for (uint8_t segment = 0; segment < NOMBRE_SEGMENTS_CERCLE; ++segment) {
        mouvementArriere(longueurSegmentCM);
        tourne(ANGLE_SEGMENT_DEGRES);
    }

    pinceFermeture();
    delay(500);
    tourne(180.0f);
}
