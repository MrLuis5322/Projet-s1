#include <LibRobus.h>
#include <pince.h>

namespace {
const uint8_t ANGLE_OUVERTURE = 0;
const uint8_t ANGLE_FERMETURE = 90;
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
