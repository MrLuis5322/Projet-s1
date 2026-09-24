#include <LibRobus.h>
#include <Son.h>

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
