/*
Projet: Labyrinthe-S1
Equipe: 13-A
Auteurs: 
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
#include <Arduino.h>

/*
Variables globales et defines
 -> defines...
 -> L'ensemble des fonctions y ont acces
*/

//********CONSTANTES*******//
#define KP 1
#define KI 0.40
#define KD 0.15
#define dt 0.01

const int vertPin = 48;
const int rougePin = 49;

//********VARIABLES********//
bool bumperArr; //variable vrai ou faux
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
int count = 0;

float vitesseLeft = 0.0;
float vitesseRight = 0.0;
int32_t countEncLeft = 0;
int32_t countEncRight = 0;

// variables PID
float pulseCibleLeft = 50.63;
float pulseCibleRight = 50;
float integrale = 0;
float derivee = 0;
float correction = 0;
float erreurPrecedenteLeft = 0;
float erreurPrecedenteRight = 0;


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
}

void avance(){
  MOTOR_SetSpeed(LEFT, vitesseLeft);
  MOTOR_SetSpeed(RIGHT, vitesseRight);
}

void recule(){
  MOTOR_SetSpeed(LEFT, -0.5*vitesseLeft);
  MOTOR_SetSpeed(RIGHT, -0.5*vitesseRight);
}

void tourneDroit(){
  MOTOR_SetSpeed(LEFT, 0.5*vitesseLeft);
  MOTOR_SetSpeed(RIGHT, -0.5*vitesseRight);
}

void tourneGauche(){
  MOTOR_SetSpeed(LEFT, -0.5*vitesseLeft);
  MOTOR_SetSpeed(RIGHT, 0.5*vitesseRight);
}

float calculPid(int pulse, float vitesse, float &erreurPrecedente, float pulseCible){
  float erreur = pulseCible - pulse;
  
  integrale += erreur * dt;
  derivee = (erreur - erreurPrecedente) / dt;
  correction = KP * erreur + KI * integrale + KD * derivee;

  erreurPrecedente = erreur;

  return vitesse + correction*0.0001; 
}

void avanceDroit(int32_t encLeft, int32_t encRight){
  avance();
  vitesseLeft = calculPid(encLeft, vitesseLeft, erreurPrecedenteLeft, pulseCibleLeft);
  vitesseRight = calculPid(encRight, vitesseRight, erreurPrecedenteRight, pulseCibleRight);
}

/*
Fonctions d'initialisation (setup)
 -> Se fait appeler au debut du programme
 -> Se fait appeler seulement un fois
 -> Generalement on y initilise les varibbles globales
*/
void setup(){
  BoardInit();
  
  pinMode(vertPin, INPUT);
  pinMode(rougePin, INPUT);
  delay(100);
  Serial.println("Initialisation complete");
}

/*
Fonctions de boucle infini
 -> Se fait appeler perpetuellement suite au "setup"
*/
void loop() {

  //Test encodeurs PID

  if(ROBUS_IsBumper(3)){
    etat = 1;
  }
  if(ROBUS_IsBumper(2)){
    etat = 0;
    arret();
  }
  if(!etat){
    return;
  }

  int32_t encLeft = ENCODER_Read(LEFT);
  int32_t encRight = ENCODER_Read(RIGHT);

  avanceDroit(encLeft, encRight);
  
  countEncLeft += encLeft;
  countEncRight += encRight;
  
  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);
  delay(10);


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
  
  // vert = digitalRead(vertPin);
  // rouge = digitalRead(rougePin);
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