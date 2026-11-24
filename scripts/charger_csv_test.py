# Prépare les données du CSV avant la compilation du Mega.
import csv
import json
from pathlib import Path

Import("env")


# Charge l'ancien CSV à quatre colonnes ou le nouveau avec Nom.
def charger_csv(chemin):
    contenu = chemin.read_bytes()
    try:
        texte = contenu.decode("utf-8-sig")
    except UnicodeDecodeError:
        texte = contenu.decode("cp1252")

    lignes = csv.reader(texte.splitlines(), delimiter=";")
    entete = next(lignes, [])
    if len(entete) not in (4, 5):
        raise ValueError("data.csv : entête invalide")
    cases = []
    for ligne in lignes:
        if not ligne:
            continue
        if len(ligne) not in (4, 5):
            raise ValueError(f"data.csv : nombre de colonnes invalide à la ligne {lignes.line_num}")
        numero, type_objet, position, etat = [champ.strip() for champ in ligne[:4]]
        nom = ligne[4].strip() if len(ligne) == 5 else ""
        champs = [position, numero, type_objet, nom, etat]
        if any("|" in champ or any(ord(c) < 32 for c in champ) for champ in champs):
            raise ValueError(f"data.csv : caractère invalide à la ligne {lignes.line_num}")
        if etat not in ("-", "0", "1", "2"):
            raise ValueError(f"data.csv : état invalide à la ligne {lignes.line_num}")
        cases.append(champs)
    return cases


# Écrit les lignes sous forme de tableau C++ dans include.
def generer_entete(cases, dossier):
    dossier.mkdir(parents=True, exist_ok=True)
    lignes = ["// Généré depuis data.csv par PlatformIO.", "Case cases[MAX_CASES] = {"]
    for case in cases:
        champs = ", ".join(json.dumps(champ, ensure_ascii=False) for champ in case)
        lignes.append("    {" + champs + "},")
    lignes.extend(["};", f"int nombreCases = {len(cases)};", ""])
    cible = dossier / "inventaire_initial.h"
    texte = "\n".join(lignes)
    if not cible.exists() or cible.read_text(encoding="utf-8") != texte:
        cible.write_text(texte, encoding="utf-8")


projet = Path(env.subst("$PROJECT_DIR"))
source = projet.parent / "API" / "Inventaire" / "data.csv"
cases = charger_csv(source)
if len(cases) > 80:
    raise ValueError("data.csv : maximum 80 lignes pour ce test Mega")
generer_entete(cases, projet / "include")
print(f"Test Bluetooth : {len(cases)} lignes chargées depuis {source}")
