/*
Projet: Le nom du script
Equipe: 13-A
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: 24/09/2026
*/


/*
Get-Location
Test-Path .\platformio.ini
Test-Path .\lib\Robus\Robus.h

*/

/*
Inclure les librairies de functions que vous voulez utiliser
*/
#include <LibRobus.h>
// a essayer #include <Robus/Robus.h>
#include <Arduino.h>

/*
Variables globales et defines
 -> defines...
 -> L'ensemble des fonctions y ont acces
*/

bool bumperArr; //variable vrai ou faux
int vertpin = 48; //nombre entier
int rougepin = 49;
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesseM1 = 0.4; //nombre avec des decimales
float vitesseM2 = 0.4; //nombre avec des decimales
float integrale = 0;
float dt = 0.05;
float erreurPrecedenteM1 = 0;
float erreurPrecedenteM2 = 0;

/*
Vos propres fonctions sont creees ici
*/

void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}

void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};

void avance(){
  MOTOR_SetSpeed(LEFT,vitesseM1);
  MOTOR_SetSpeed(RIGHT, vitesseM2);
};

void recule(){
  MOTOR_SetSpeed(LEFT, -0.5*vitesseM1);
  MOTOR_SetSpeed(RIGHT, -0.5*vitesseM2);
};

void tourneDroit(){
  MOTOR_SetSpeed(LEFT, 0.5*vitesseM1);
  MOTOR_SetSpeed(RIGHT, -0.5*vitesseM2);
};

void tourneGauche(){
  MOTOR_SetSpeed(LEFT, -0.5*vitesseM1);
  MOTOR_SetSpeed(RIGHT, 0.5*vitesseM2);
};

float calculPid(int pulse, float vitesse, float &erreurPrecedente){
  float pulseCible = 1000;
  float erreur = pulseCible - pulse;
  float Kp = 0.4;
  float Ki = 0.02;
  float Kd = 0.005;

  integrale += erreur * dt;

  float derivee = (erreur - erreurPrecedente) / dt;

  float correction = Kp * erreur + Ki * integrale + Kd * derivee;

  erreurPrecedente = erreur;

  return vitesse + correction*0.0001; 

};

/*
Fonctions d'initialisation (setup)
 -> Se fait appeler au debut du programme
 -> Se fait appeler seulement un fois
 -> Generalement on y initilise les varibbles globales
*/
void setup(){
  BoardInit();
  
  //initialisation
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);
  beep(3);
  Serial.println("Initialisation complete");
}

/*
Fonctions de boucle infini
 -> Se fait appeler perpetuellement suite au "setup"
*/
void loop() {

  //Test encodeurs
  MOTOR_SetSpeed(LEFT,vitesseM1);
  MOTOR_SetSpeed(RIGHT,vitesseM2);
  int32_t enc1 = ENCODER_Read(0);
  int32_t enc2 = ENCODER_Read(1);
  Serial.print("Encoder 1: ");
  Serial.println(enc1);
  Serial.print("Encoder 2: ");
  Serial.println(enc2);
  vitesseM1 = calculPid(enc1, vitesseM1, erreurPrecedenteM1);
  vitesseM2 = calculPid(enc2, vitesseM2, erreurPrecedenteM2);
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  delay(100);


  // etatPast = etat;
  // bumperArr = ROBUS_IsBumper(3);
  // if (bumperArr){
  //   if (etat == 0){
  //     beep(2);
  //     etat = 1;
  //   } 
  //   else{
  //     beep(1);
  //     etat = 0;
  //   }
  // }
  
  // vert = digitalRead(vertpin);
  // rouge = digitalRead(rougepin);
  // if (etat > 0){
  //   if (vert && rouge){ // aucun obstacle => avance
  //     etat = 1;
  //   }
  //   if (!vert && !rouge){  // obstacle devant => recule
  //     etat = 2;
  //   }
  //   if (!vert && rouge){ // obstacle à gauche => tourne droit
  //       etat = 3;
  //     }
  //   if (vert && !rouge){ // obstacle à droite => tourne gauche
  //       etat = 4;
  //   }
  // }

  // if (etatPast != etat){
  //   arret();
  //   delay(50);
  // }
  // else{
  //   switch (etat)
  //   {
  //   case 0:
  //     arret();
  //     break;
  //   case 1:
  //     avance();
  //     break;
  //   case 2:
  //     recule();
  //     break;
  //   case 3:
  //     tourneDroit();
  //     break;
  //   case 4:
  //     tourneGauche();
  //     break;            
  //   default:
  //     avance();
  //     etat = 1;
  //   break;
  //   }
  // }
  // delay(200);
}
