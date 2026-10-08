/*
Algorithme - P13
Auteurs: Olivier Gélinas
Projet: Défi du parcours
Equipe: 13
Description: Script qui permet au robot de parcourir le labyrinthe
Date: 2026-10-01
*/

// Librairies de fonctions à utiliser

#include <Arduino.h>
#include <LibRobus.h>
#include "Mouvement.h"

int x;                 // Position horizontale dans la grille {0; 1; 2}
int y;                 // Position verticale dans la grille {0; 1; 2; 3; 4; 5; 6; 7; 8; 9}
int c;                 // Colonne (x) souhaitée dans le contexte où le robot doit reculer
int nbChoixAleatoires; // Alternance Droite-Gauche
int memoire[3][10][5]; // Pour chaque case (0 ou 1) : {MurAvant; MurGauche; MurArriere; MurDroite; CaseVisitée}
int direction;         // {0 : Avant ; 1 : Gauche ; 2 : Arriere ; 3 : Droite}

void setup()
{
    // Initialisation
    x = 1;
    y = 0;
    c = 0;
    nbChoixAleatoires = 0;
    direction = 0;
    InitialiserMemoire(); // Remplir le contenu du tableau

    

    Devant();

    beep(3);
}

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

/*
Fonction : Remplir le contenu qui contient les informations sur les cases.
Entrée : Rien
Retour : Rien.
*/
void InitialiserMemoire()
{
    int valeurs[3][10][5] = {// Contenu initial souhaité
                             {{0, 1, 1, 0, 0},
                              {0, 1, 0, 1, 0},
                              {0, 1, 0, 0, 0},
                              {0, 1, 0, 1, 0},
                              {0, 1, 0, 0, 0},
                              {0, 1, 0, 1, 0},
                              {0, 1, 0, 0, 0},
                              {0, 1, 0, 1, 0},
                              {0, 1, 0, 0, 0},
                              {1, 1, 0, 1, 0}},

                             {{0, 0, 1, 0, 1},
                              {0, 1, 0, 1, 0},
                              {0, 0, 0, 0, 0},
                              {0, 1, 0, 1, 0},
                              {0, 0, 0, 0, 0},
                              {0, 1, 0, 1, 0},
                              {0, 0, 0, 0, 0},
                              {0, 1, 0, 1, 0},
                              {0, 0, 0, 0, 0},
                              {1, 1, 0, 1, 0}},

                             {{0, 0, 1, 1, 0},
                              {0, 1, 0, 1, 0},
                              {0, 0, 0, 1, 0},
                              {0, 1, 0, 1, 0},
                              {0, 0, 0, 1, 0},
                              {0, 1, 0, 1, 0},
                              {0, 0, 0, 1, 0},
                              {0, 1, 0, 1, 0},
                              {0, 0, 0, 1, 0},
                              {1, 1, 0, 1, 0}}};

    // Remplissage de la mémoire
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < 5; k++)
            {
                memoire[i][j][k] = valeurs[i][j][k];
            }
        }
    }
}

/*
Fonction : Lire le détecteur de proximité et retourner un booléen en fonction de la présence d'un mur ou non.
Entrée : Rien
Retour : Booléen true si il y a un mur devant le robot. Sinon, false.
*/
bool CapteurDetecteMur()
{
    int vert = digitalRead(48);
    int rouge = digitalRead(49);

    if(!vert && !rouge) return true;
    else return false;
}

/*
Fonction : Retourner un booléen en fonction de la présence d'un mur devant le robot.
Entrée : Rien
Retour : Booléen true si il y a un mur ou une ligne de tape devant le robot. Sinon, false.
*/
bool MurDevant()
{
    if (memoire[x][y][direction] == 1)
        return true;
    else if (CapteurDetecteMur()){
        memoire[x][y][direction] = 1;
        return true;
    }
    else
        return false;
}

/*
Fonction : Faire avancer le robot d'une distance voulue dans la direction dans laquelle fait face le robot.
           Elle s'assure aussi de modifier les variables globales de position actuelle et la mémoire.
Entrée : Nombre de cases à avancer
Retour : Rien.
*/
void Avancer(int p_nbCases)
{
    avance(p_nbCases * 0.5);

    switch (direction)
    {
    case 0:
        y += p_nbCases;
        break;
    case 1:
        x -= p_nbCases;
        break;
    case 2:
        y -= p_nbCases;
        break;
    case 3:
        x += p_nbCases;
        break;
    }

    memoire[x][y][4] == 1;
}

/*
Fonction : Faire tourner le robot d'un angle voulu.
           Elle s'assure aussi de modifier la variable direction, qui garde en mémoire l'orientation actuelle du robot.
Entrée : Angle de rotation à effectuer, soit un multiple de 90 degrés. Une rotation antihoraire est positive.
Retour : Rien.
*/
void Tourner(int p_angle)
{
    if (p_angle < 0) {
        int p_direction = RIGHT;
    }
    else if (p_angle > 0) {
        int p_direction = LEFT;
    }
    int nbQuartTourATourner = p_angle / 90;

    for (int i = 0; i < abs(nbQuartTourATourner); i++){
        rotation90(LEFT);
    }

    // FAIRE TOURNER LE ROBOT ICI

    direction += nbQuartTourATourner;
    direction = ((direction % 4) + 4) % 4;
}

/*
Fonction : Dicter une séquence de mouvements lorsque le robot est immobile, face vers l'avant,
           et qu'il n'a aucune information quant au tracé du labyrinthe à la hauteur où il se trouve.
Entrée : Rien
Retour : Rien.
*/
void Devant()
{
    if (!MurDevant()) 
    {
        if (y == 8) // Cas où le robot est à une case de la fin
            Avancer(1);
        else 
            Avancer(2);
        Devant();
    }
    else
    {
        if (y != 9)
        {
            if (x == 0)
                Droite();
            else if (x == 2)
                Gauche();
            else if (x == 1) // Si le robot est au centre, on utilise la logique suivante 
                             // afin qu'il ne choisisse pas le même côté que précédemment.
            { // Alternance Droite-Gauche
                nbChoixAleatoires++;
                if (nbChoixAleatoires % 2 == 0)
                    Droite();
                else
                    Gauche();
            }
        }
    }
}

/*
Fonction : Dicter une séquence de mouvements lorsque le robot est immobile, face vers l'avant,
           et qu'il sait qu'il doit s'en aller vers sa gauche.
Entrée : Rien
Retour : Rien.
*/
void Gauche()
{
    Tourner(90);
    if (MurDevant()) // Si présence d'un mur à gauche, alors aller plutôt à droite
    {
        Tourner(-90);
        Droite();
    }
    else
    {
        Avancer(1);
        MurDevant(); // Noter dans la mémoire la présence d'un mur à gauche ou non
        Tourner(-90);
        if (!MurDevant())
            Devant();
        else // Impossible de continuer à monter
        {
            if (memoire[x][y][1] == 0) // Si pas de mur à sa gauche
                Gauche();
            else
            {
                if (memoire[x + 1][y][3] == 0) // Si la case de droite n'a pas de mur (ou inconnu) à sa droite
                    Droite(); // Aller à droite
                else // Sinon, il faut reculer
                {
                    for (int i = 0; i < 3; i++) // Noter quelle case de cette hauteur n'a pas été visitée
                    {
                        if (memoire[i][y][4] == 0)
                        {
                            c = i; // On l'assigne comme colonne objectif
                            break;
                        }
                    }
                    Tourner(180);
                    Arriere();
                }
            }
        }
    }
}

/*
Fonction : Dicter une séquence de mouvements lorsque le robot est immobile, face vers l'avant,
           et qu'il sait qu'il doit s'en aller vers sa droite.
Entrée : Rien
Retour : Rien.
*/
void Droite()
{
    Tourner(-90);
    if (MurDevant())  // Si présence d'un mur à droite, alors aller plutôt à gauche
    {
        Tourner(90);
        Gauche();
    }
    else
    {
        Avancer(1);
        MurDevant();  // Noter dans la mémoire la présence d'un mur à droite ou non
        Tourner(90);
        if (!MurDevant())
            Devant();
        else
        {
            if (memoire[x][y][3] == 0) // Si pas de mur à sa droite
                Droite();
            else
            {
                if (memoire[x - 1][y][1] == 0)  // Si la case de gauche n'a pas de mur (ou inconnu) à sa gauche
                    Gauche();  // Aller à gauche
                else  // Sinon, il faut reculer
                {
                    for (int i = 0; i < 3; i++)
                    {
                        if (memoire[i][y][4] == 0) // Noter quelle case de cette hauteur n'a pas été visitée
                        {
                            c = i; // On l'assigne comme colonne objectif
                            break;
                        }
                    }
                    Tourner(180);
                    Arriere();
                }
            }
        }
    }
}

/*
Fonction : Dicter une séquence de mouvements lorsque le robot est immobile, face vers l'arrière,
           et qu'il sait qu'il doit revenir vers l'arrière afin de poursuivre son chemin.
Entrée : Rien
Retour : Rien.
*/
void Arriere()
{
    if (!MurDevant())
    {
        Avancer(2);
        MurDevant(); // Noter la présence d'un mur à l'arrière
        if (c == 0) // Cas où on souhaite aller dans la colonne de gauche
        {
            Tourner(-90);
            if (!MurDevant())
            {
                Avancer(1);
                if (x = c) // Si on se trouve dans la colonne souhaitée
                {
                    Tourner(-90);
                    Devant();
                }
                else if (!MurDevant()) // Sinon, continuer à avancer
                {
                    Avancer(1);
                    if (x = c)
                    {
                        Tourner(-90);
                        Devant();
                    }
                }
            }
            else if (memoire[x][y][2] == 0)
            {
                Tourner(90);
                Arriere();
            }
        }
        if (c == 2)  // Cas où on souhaite aller dans la colonne de droite
        {
            Tourner(90);
            if (!MurDevant())
            {
                Avancer(1);
                if (x = c) // Si on se trouve dans la colonne souhaitée
                {
                    Tourner(90);
                    Devant();
                }
                else if (!MurDevant()) // Sinon, continuer à avancer
                {
                    Avancer(1);
                    if (x = c)
                    {
                        Tourner(90);
                        Devant();
                    }
                }
            }
            else if (memoire[x][y][2] == 0)
            {
                Tourner(-90);
                Arriere();
            }
        }
    }
}