#!/usr/bin/env python3
# Cinder City — régénère la GALERIE de captures dans le README, à partir des
# fichiers de docs/screenshots/. La grille est réécrite entre les marqueurs
# (<!-- GALLERY:START --> ... <!-- GALLERY:END -->), la plus récente en premier.
#
# Usage MANUEL :  python3 .github/scripts/update_gallery.py
# (Fonctionne depuis n'importe quel dossier : le script retrouve la racine seul.)
#
# Convention de nommage attendue :  AAAA-MM-JJ_description-libre.png
#   -> la date sert au tri, la description (tirets/underscores) devient la légende.

import os
import re

# Racine du dépôt = deux niveaux au-dessus de ce script (.github/scripts/...).
REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
os.chdir(REPO_ROOT)

SHOTS_DIR = "docs/screenshots"
COLS = 2   # nombre de captures par ligne de la grille
EXITS = (".png", ".jpg", ".jpeg", ".gif")

def parse(name):
    """De 'AAAA-MM-JJ_desc.png' tire (date, légende, nom). Ignore le reste."""
    base = os.path.splitext(name)[0]
    m = re.match(r"(\d{4}-\d{2}-\d{2})[_-]?(.*)", base)
    if not m:
        return None
    date, desc = m.groups()
    desc = desc.replace("-", " ").replace("_", " ").strip()
    if desc:
        desc = desc[0].upper() + desc[1:]   # majuscule initiale (plus propre)
    return date, desc, name

def main():
    files = [f for f in os.listdir(SHOTS_DIR) if f.lower().endswith(EXITS)]
    items = [p for p in (parse(f) for f in files) if p]
    # Tri décroissant : la capture la plus récente d'abord.
    items.sort(key=lambda i: (i[0], i[2]), reverse=True)

    # Une cellule de tableau par capture (image cliquable + légende datée).
    width = 100 // COLS
    cells = []
    for date, desc, name in items:
        caption = f"<b>{date}</b>" + (f" — {desc}" if desc else "")
        cells.append(
            f'<td align="center" width="{width}%">'
            f'<a href="{SHOTS_DIR}/{name}"><img src="{SHOTS_DIR}/{name}" width="100%"></a>'
            f'<br><sub>{caption}</sub></td>'
        )

    # On regroupe les cellules par lignes de COLS.
    lines = ["<table>"]
    for k in range(0, len(cells), COLS):
        lines.append("  <tr>" + "".join(cells[k:k + COLS]) + "</tr>")
    lines.append("</table>")
    table = "\n".join(lines) if cells else "<sub>(aucune capture pour l'instant)</sub>"

    block = "<!-- GALLERY:START -->\n" + table + "\n<!-- GALLERY:END -->"

    with open("README.md", encoding="utf-8") as fh:
        readme = fh.read()

    pattern = re.compile(r"<!-- GALLERY:START -->.*?<!-- GALLERY:END -->", re.DOTALL)
    if not pattern.search(readme):
        raise SystemExit("Marqueurs GALLERY introuvables dans README.md")
    readme = pattern.sub(lambda _: block, readme)   # lambda : évite l'interprétation des '\'

    with open("README.md", "w", encoding="utf-8") as fh:
        fh.write(readme)

    print(f"Galerie : {len(items)} capture(s), {COLS} par ligne.")

if __name__ == "__main__":
    main()
