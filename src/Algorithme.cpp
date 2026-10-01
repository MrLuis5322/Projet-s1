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
#include "DetecteurProximite.h"
#include "Mouvement.h"
#include "Son.h"

int x;                 // Position horizontale dans la grille {0; 1; 2}
int y;                 // Position verticale dans la grille {0; 1; 2; 3; 4; 5; 6; 7; 8; 9}
int memoire[3][10][5]; // Pour chaque case (0 ou 1) : {MurAvant; MurGauche; MurArriere; MurDroite; CaseVisitée}
int direction;         // {0 : Avant ; 1 : Gauche ; 2 : Arriere ; 3 : Droite}

int main()
{
    // Initialisation
    x = 1;
    y = 0;
    direction = 0;
    InitialiserMemoire(); // Remplir le contenu du tableau

    // ATTENDRE LE BEEP

    //Avant();

    return EXIT_SUCCESS;
}

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

                             {{0, 0, 1, 0, 0},
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

// A COMPLETER
/*
Fonction : Retourner un booléen en fonction de la présence d'un mur devant le robot.
Entrée : Rien
Retour : Booléen true si il y a un mur ou une ligne de tape devant le robot. Sinon, false.
*/
bool Mur()
{
    if (memoire[x][y][direction] == 1)
        return true;
    /*
    else if (CAPTEUR DETECTE MUR){
        memoire[x][y][direction] = 1
        return true;
    }*/
    else
        return false;
}






/*
void Droite()
{
    // Rotate -90deg
    if (Mur())
    {
        // Rotate 90 deg
        Gauche();
    }
    else
    {
        // Avance 1 case
        x++;
        // Rotate 90 deg
        if (Mur())
        {
            if (x == 1)
            {
                Droite();
            }
            else if (x == 2)
            {
                Gauche();
            }
            else
            {
                // Erreur
            }
        }
        else
        {
            Avant();
        }
    }
}

void Gauche()
{
    // Rotate 90deg
    if (Mur())
    {
        // Rotate -90 deg
        Droite();
    }
    else
    {
        // Avance 1 case
        x--;
        // Rotate -90 deg
        if (Mur())
        {
            if (x == 0)
            {
                Droite();
            }
            else if (x == 1)
            {
                Gauche();
            }
            else
            {
                // Erreur
            }
        }
        else
        {
            Avant();
        }
    }
}

void Avant()
{
    if (Mur())
    {
        if (y == 9)
        {
            return;
        }
        else
        {
            if (x == 0)
            {
                Droite();
            }
            else if (x == 1)
            {
                // RANDOM
            }
            else if (x == 2)
            {
                Gauche();
            }
            else
            {
                // Erreur
            }
        }
    }
    else
    {
        if (y == 9)
        {
            return;
        }
        else if (y != 8)
        {
            // Avance 2 cases
            y += 2;
        }
        else if (y == 8)
        {
            // Avance 1 case
            y++;
        }
        else
        {
            // Erreur
        }
    }
}
*/