#!/usr/bin/env python3
# Cinder City — compte les lignes de code du projet et met à jour le bloc
# marqué (<!-- LOC:START --> ... <!-- LOC:END -->) dans le README.
# Usage MANUEL :  python3 .github/scripts/update_loc.py
# (Fonctionne depuis n'importe quel dossier : le script retrouve la racine du
#  dépôt tout seul. La mise à jour du README part ensuite dans ton commit.)

import os
import re

# Racine du dépôt = deux niveaux au-dessus de ce script (.github/scripts/...).
# On s'y place d'entrée : tous les chemins relatifs (src/, README.md...) sont
# ainsi corrects quel que soit le dossier d'où on lance le script.
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
os.chdir(REPO_ROOT)

# Catégories : (libellé, dossier racine, extensions)
CATEGORIES = [
    ("C++ (`src/`)",   "src",       (".cpp", ".hpp", ".h")),
    ("Shaders (GLSL)", "shaders",   (".vert", ".frag")),
    ("CMake",          ".",         ("CMakeLists.txt",)),  # fichier précis
]

def iter_files(root, exts):
    """Renvoie les fichiers sous `root` dont le nom finit par une des extensions."""
    if root == "." :
        # cas spécial : on ne prend que le CMakeLists.txt de la racine
        for e in exts:
            if os.path.isfile(e):
                yield e
        return
    for base, _dirs, files in os.walk(root):
        for f in files:
            if f.endswith(exts):
                yield os.path.join(base, f)

def count_lines(path):
    with open(path, encoding="utf-8", errors="replace") as fh:
        return sum(1 for _ in fh)

def main():
    rows = []
    total_files = 0
    total_lines = 0
    for label, root, exts in CATEGORIES:
        files = list(iter_files(root, exts))
        lines = sum(count_lines(p) for p in files)
        rows.append((label, len(files), lines))
        total_files += len(files)
        total_lines += lines

    # Construit le bloc Markdown.
    badge = f"![Lignes de code](https://img.shields.io/badge/lignes_de_code-{total_lines}-2EA043?style=for-the-badge)"
    table = ["| Catégorie | Fichiers | Lignes |", "|---|---:|---:|"]
    for label, nf, nl in rows:
        table.append(f"| {label} | {nf} | {nl} |")
    table.append(f"| **Total** | **{total_files}** | **{total_lines}** |")

    block = (
        "<!-- LOC:START -->\n"
        f"{badge}\n\n"
        + "\n".join(table) + "\n\n"
        "<sub>Pour mettre à jour ce compteur, depuis la racine du dépôt :</sub>\n\n"
        "```bash\n"
        "python3 .github/scripts/update_loc.py\n"
        "```\n"
        "<!-- LOC:END -->"
    )

    with open("README.md", encoding="utf-8") as fh:
        readme = fh.read()

    # Remplace le bloc existant entre les marqueurs (regex "non gourmande").
    pattern = re.compile(r"<!-- LOC:START -->.*?<!-- LOC:END -->", re.DOTALL)
    if pattern.search(readme):
        readme = pattern.sub(block, readme)
    else:
        # Pas encore de bloc : on l'ajoute à la fin.
        readme = readme.rstrip() + "\n\n" + block + "\n"

    with open("README.md", "w", encoding="utf-8") as fh:
        fh.write(readme)

    print(f"LOC : {total_lines} lignes, {total_files} fichiers")

if __name__ == "__main__":
    main()
