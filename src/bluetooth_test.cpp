#include <Arduino.h>

// Test Bluetooth. Inventaire fictif en RAM. Aucun appel moteur ni carte SD.
const unsigned long DEBIT_BT = 9600;
const int MAX_CASES = 80;
const unsigned int MAX_LIGNE = 200;

struct Case {
    String position;
    String numero;
    String type;
    String nom;
    String etat = "-";

    // Case vide réservée dans le tableau.
    Case() = default;

    // Charge une ligne générée depuis le CSV.
    Case(const char* position, const char* numero, const char* type,
         const char* nom, const char* etat)
        : position(position), numero(numero), type(type), nom(nom), etat(etat) {}
};

// Le fichier généré contient les cases copiées du CSV local.
#include "inventaire_initial.h"
bool arretActif = false;
String ligneRecue;
bool ligneTropLongue = false;

// Renvoie une ligne au PC et la montre aussi dans le moniteur USB.
void envoyer(const String& ligne) {
    Serial1.println(ligne);
    Serial.print(F("BT > "));
    Serial.println(ligne);
}

// Renvoie l'identifiant reçu pour que le PC reconnaisse sa réponse.
void accuser(const String& id, const String& erreur = "") {
    if (erreur.length() == 0) {
        envoyer("ACK|" + id + "|OK");
    } else {
        envoyer("ACK|" + id + "|ERROR|" + erreur);
    }
}

// Envoie le format commun aux lignes PIECE et MATCH.
void envoyerCase(const String& message, const Case& emplacement) {
    if (emplacement.etat == "-") {
        envoyer(message + "|---|-|" + emplacement.position + "|-|");
    } else {
        envoyer(message + "|" + emplacement.numero + "|" + emplacement.type + "|" +
                emplacement.position + "|" + emplacement.etat + "|" + emplacement.nom);
    }
}

// Envoie une copie complète à l'onglet Inventaire.
void envoyerInventaire() {
    envoyer("INV_BEGIN");
    for (int i = 0; i < nombreCases; i++) {
        envoyerCase("PIECE", cases[i]);
    }
    envoyer("INV_END");
}

// Trouve une case unique. -1 absente, -2 en double dans le CSV.
int trouverCase(const String& position) {
    int index = -1;
    for (int i = 0; i < nombreCases; i++) {
        if (cases[i].position == position) {
            if (index >= 0) {
                return -2;
            }
            index = i;
        }
    }
    return index;
}

// Vide un objet, garde la position.
void viderCase(Case& emplacement) {
    emplacement.numero = "";
    emplacement.type = "";
    emplacement.nom = "";
    emplacement.etat = "-";
}

// Retour initial du test. 75 % et 12.1 V sont des valeurs fictives.
void synchroniser(const String& id) {
    accuser(id);
    envoyer(arretActif ? "ETAT|ARRET" : "ETAT|PRET");
    envoyer("BATTERIE|75|12.1");
    envoyer("INFO|MODE_TEST_REPONSES_SIMULEES");
    envoyerInventaire();
}

// Filtre les objets disponibles par numéro, type, position ou nom.
void chercher(const String& id, const String& critere, String valeur) {
    if (critere != "NUMERO" && critere != "TYPE_TEXTE" && critere != "POSITION" && critere != "NOM") {
        accuser(id, "CRITERE_INCONNU");
        return;
    }
    accuser(id);
    envoyer("MATCH_BEGIN");
    for (int i = 0; i < nombreCases; i++) {
        const Case& emplacement = cases[i];
        if (emplacement.etat != "1" || trouverCase(emplacement.position) < 0) {
            continue;
        }
        String nom = emplacement.nom;
        String type = emplacement.type;
        // String compare les lettres ASCII sans casse. Accents comparés tels quels.
        nom.toLowerCase();
        type.toLowerCase();
        String texte = valeur;
        texte.toLowerCase();
        if ((critere == "NUMERO" && emplacement.numero == valeur) ||
            (critere == "POSITION" && emplacement.position == valeur) ||
            (critere == "TYPE_TEXTE" && type == texte) ||
            (critere == "NOM" && nom.indexOf(texte) >= 0)) {
            envoyerCase("MATCH", emplacement);
        }
    }
    envoyer("MATCH_END");
}

// Vérifie la destination, puis simule le dépôt dans une case.
void stocker(const String& id, const String& numero, const String& type,
             const String& position, const String& nom) {
    if (nom.length() == 0 || type.length() == 0) {
        accuser(id, "NOM_OU_TYPE_MANQUANT");
        return;
    }
    int index = trouverCase(position);
    for (int i = 0; i < nombreCases; i++) {
        if (numero != "-" && cases[i].numero == numero) {
            accuser(id, "NUMERO_DEJA_UTILISE");
            return;
        }
        if (position == "-" && index < 0 && cases[i].etat == "-" &&
            trouverCase(cases[i].position) == i) {
            index = i;
        }
    }
    if (index < 0 || cases[index].etat != "-") {
        accuser(id, "AUCUNE_CASE_DISPONIBLE");
        return;
    }
    // Version réelle : déposer à cases[index].position puis confirmer la réussite.
    cases[index].numero = numero == "-" ? "---" : numero;
    cases[index].type = type;
    cases[index].nom = nom;
    cases[index].etat = "1";
    accuser(id);
    envoyer("INFO|STOCKAGE_SIMULE|" + cases[index].position);
    envoyerInventaire();
}

// Reçoit la position de l'objet choisi et simule son retrait.
void recuperer(const String& id, const String& position) {
    int index = trouverCase(position);
    if (index < 0 || cases[index].etat != "1") {
        accuser(id, "PIECE_NON_DISPONIBLE");
        return;
    }
    // Version réelle : aller à position, saisir l'objet, confirmer la réussite.
    String nom = cases[index].nom.length() ? cases[index].nom : cases[index].numero;
    viderCase(cases[index]);
    accuser(id);
    envoyer("INFO|RETRAIT_SIMULE|" + nom + "|" + position);
    envoyerInventaire();
}

// Ajoute une case fictive en fin de liste.
void ajouterEmplacement(const String& id, const String& position) {
    if (position.length() == 0 || trouverCase(position) != -1) {
        accuser(id, "POSITION_INVALIDE_OU_EXISTANTE");
        return;
    }
    if (nombreCases == MAX_CASES) {
        accuser(id, "LIMITE_DU_TEST");
        return;
    }
    cases[nombreCases].position = position;
    viderCase(cases[nombreCases]);
    nombreCases++;
    accuser(id);
    envoyerInventaire();
}

// Retire seulement une case vide.
void retirerEmplacement(const String& id, const String& position) {
    int index = trouverCase(position);
    if (index < 0 || cases[index].etat != "-") {
        accuser(id, "POSITION_INCONNUE_OU_OCCUPEE");
        return;
    }
    for (int i = index; i < nombreCases - 1; i++) {
        cases[i] = cases[i + 1];
    }
    nombreCases--;
    accuser(id);
    envoyerInventaire();
}

// Vide les objets fictifs. Garde toutes les positions.
void resetInventaire(const String& id) {
    for (int i = 0; i < nombreCases; i++) {
        viderCase(cases[i]);
    }
    accuser(id);
    envoyerInventaire();
}

// Simule l'arrêt ou le réarmement. Aucun effet sur le matériel.
void changerArret(const String& id, bool arret) {
    arretActif = arret;
    accuser(id);
    envoyer(arret ? "ETAT|ARRET" : "ETAT|PRET");
}

// Découpe une commande sur |. Garde les champs vides pour la validation.
void traiterCommande(const String& ligne) {
    String champs[7];
    unsigned int nombre = 0;
    int debut = 0;
    for (unsigned int i = 0; i <= ligne.length(); i++) {
        if (i == ligne.length() || ligne[i] == '|') {
            if (nombre == 7) {
                envoyer("ERROR|TROP_DE_CHAMPS");
                return;
            }
            champs[nombre++] = ligne.substring(debut, i);
            debut = i + 1;
        }
    }
    if (nombre < 3 || champs[0] != "CMD" || champs[1].length() == 0) {
        envoyer("ERROR|FORMAT_COMMANDE");
        return;
    }
    const String& id = champs[1];
    const String& action = champs[2];
    if (action == "SYNC" && nombre == 3) synchroniser(id);
    else if (action == "EMERGENCY_STOP" && nombre == 3) changerArret(id, true);
    else if (action == "RESET_ESTOP" && nombre == 3) changerArret(id, false);
    else if (arretActif) accuser(id, "ARRET_ACTIF");
    else if (action == "CHERCHER" && nombre == 5) chercher(id, champs[3], champs[4]);
    else if (action == "STOCKER" && nombre == 7) stocker(id, champs[3], champs[4], champs[5], champs[6]);
    else if (action == "RECUPERER" && nombre == 4) recuperer(id, champs[3]);
    else if (action == "AJOUTER_EMPLACEMENT" && nombre == 4) ajouterEmplacement(id, champs[3]);
    else if (action == "RETIRER_EMPLACEMENT" && nombre == 4) retirerEmplacement(id, champs[3]);
    else if (action == "RESET_INVENTAIRE" && nombre == 3) resetInventaire(id);
    else accuser(id, "COMMANDE_OU_ARGUMENTS_INVALIDES");
}

// Ouvre USB pour le journal et Serial1 pour le module.
void setup() {
    Serial.begin(115200);
    Serial1.begin(DEBIT_BT);
    ligneRecue.reserve(MAX_LIGNE);
    Serial.println(F("Test Bluetooth pret sur Serial1"));
}

// Assemble les octets jusqu'au saut de ligne. Aucun délai bloquant en lecture.
void loop() {
    while (Serial1.available() > 0) {
        char caractere = Serial1.read();
        if (caractere == '\r') continue;
        if (caractere == '\n') {
            if (ligneTropLongue) envoyer("ERROR|LIGNE_TROP_LONGUE");
            else if (ligneRecue.length() > 0) {
                Serial.print(F("BT < "));
                Serial.println(ligneRecue);
                traiterCommande(ligneRecue);
            }
            ligneRecue = "";
            ligneTropLongue = false;
        } else if (ligneRecue.length() < MAX_LIGNE) {
            ligneRecue += caractere;
        } else {
            ligneTropLongue = true;
        }
    }
}
