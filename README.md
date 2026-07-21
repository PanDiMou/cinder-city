<img src="assets/banner.svg" width="100%" alt="Cinder City">

### *Un monde ouvert. Un développeur. Une obsession.*

Un jeu vidéo en monde ouvert urbain, construit intégralement à la main —
sans moteur du commerce, une ligne de code après l'autre.

<p align="center">
  <img src="docs/screenshots/2026-07-21_avenue.png" width="90%" alt="Cinder City — premier aperçu d'une avenue">
  <br>
  <sub><i>🚧 En cours de développement — premier aperçu : une avenue rendue par le moteur maison (éditeur intégré visible à gauche).</i></sub>
</p>

![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![SDL_gpu](https://img.shields.io/badge/SDL__gpu-rendu-AC162C?style=for-the-badge)
![SDL3](https://img.shields.io/badge/SDL-3.x-1D4ED8?style=for-the-badge)
![CMake](https://img.shields.io/badge/CMake-build-064F8C?style=for-the-badge&logo=cmake&logoColor=white)

![Status](https://img.shields.io/badge/statut-en%20développement-F5A623?style=flat-square)
![Solo Dev](https://img.shields.io/badge/équipe-1%20développeur-FF6B35?style=flat-square)
![Map](https://img.shields.io/badge/monde-1%20km²-2EA043?style=flat-square)
![License](https://img.shields.io/badge/licence-tous%20droits%20réservés-D63A2F?style=flat-square)

<!-- LOC:START -->
![Lignes de code](https://img.shields.io/badge/lignes_de_code-2990-2EA043?style=for-the-badge)

| Catégorie | Fichiers | Lignes |
|---|---:|---:|
| C++ (`src/`) | 38 | 2706 |
| Shaders (GLSL) | 6 | 148 |
| CMake | 1 | 136 |
| **Total** | **45** | **2990** |

<sub>Pour mettre à jour ce compteur, depuis la racine du dépôt :</sub>

```bash
python3 .github/scripts/update_loc.py
```
<!-- LOC:END -->

<img src="assets/divider.svg" width="100%" alt="">

> *« Pas de moteur clé en main. Pas de raccourci.*
> *Juste du code, de la patience, et un monde à faire vivre. »*

<img src="assets/divider.svg" width="100%" alt="">

## 🎮 Le projet

**Cinder City** est un jeu en monde ouvert : une ville d'un kilomètre carré,
vivante et entièrement explorable — véhicules, circulation, foule et systèmes
réactifs à grande échelle.

Le pari est assumé : **tout construire soi-même**. Chaque brique — fenêtrage,
rendu, physique, audio, IA des habitants — est écrite sur mesure. Pour un
contrôle total, des performances au plus près du métal, et un code taillé pour
durer.

## 🔥 Une aventure en solo

Cinder City est développé par **un seul développeur**, porté par une conviction
simple : un monde ouvert ambitieux peut naître d'un travail patient, rigoureux
et passionné. L'ambition est grande. L'approche est méthodique. La détermination
est totale.

C'est un projet au long cours — et il est mené comme tel.

## ✨ Vision

| | |
|---|---|
| 🗺️ **Monde ouvert** | 1 km × 1 km explorables, sans chargement apparent |
| 🚗 **Conduite libre** | Véhicules et déplacement au cœur de l'expérience |
| 🏙️ **Ville vivante** | Circulation, piétons et systèmes réactifs simulés en masse |
| ⚡ **Performance** | Architecture soignée et C++ moderne, pensés pour tenir dans le temps |

<img src="assets/divider.svg" width="100%" alt="">

## 🛠️ Stack technique

Un socle **C++23 moderne**, sans moteur, assemblé autour de technologies
éprouvées et pérennes. `✅` = intégré · `🔜` = prévu.

| Domaine | Technologie | Rôle | Statut |
| --- | --- | --- | :---: |
| 🪟 Fenêtrage / entrées | ![SDL3](https://img.shields.io/badge/SDL3-1D4ED8?style=flat-square) | Fenêtres et périphériques | ✅ |
| 🎨 Rendu 3D | ![SDL_gpu](https://img.shields.io/badge/SDL__gpu-AC162C?style=flat-square) | Rendu moderne cross-platform (Metal / Vulkan / D3D12) | ✅ |
| 🧬 Shaders | ![shadercross](https://img.shields.io/badge/SDL__shadercross-AC162C?style=flat-square) | Traduction SPIR-V → format natif au runtime | ✅ |
| 📐 Mathématiques | ![GLM](https://img.shields.io/badge/GLM-5586A4?style=flat-square) | Algèbre linéaire 3D (matrices, quaternions) | ✅ |
| 📦 Import de modèles | ![ufbx](https://img.shields.io/badge/ufbx-6C4AB6?style=flat-square) | Chargement des modèles FBX (mètres, Y-up, UV) | ✅ |
| 🖼️ Textures | ![stb_image](https://img.shields.io/badge/stb__image-5586A4?style=flat-square) | Décodage des images (PNG / TGA) vers la VRAM | ✅ |
| 🌃 Émissif | ![GLSL](https://img.shields.io/badge/GLSL-4B8BBE?style=flat-square) | Fenêtres et enseignes lumineuses (palette + émissif additionnés) | ✅ |
| 🗺️ Scènes (données) | ![JSON](https://img.shields.io/badge/nlohmann/json-3B5998?style=flat-square) | Ville décrite en données (`city.json`), hors du code | ✅ |
| 🧩 Entités | ![OOP](https://img.shields.io/badge/OOP-6C4AB6?style=flat-square) | Modèle game object maison (transform · entity · world) | ✅ |
| 🧭 Éditeur in-game | ![Dear ImGui](https://img.shields.io/badge/Dear_ImGui-FF9800?style=flat-square) | Poser / sélectionner / éditer / sauver la ville à la souris | ✅ |
| 💥 Physique | ![Jolt](https://img.shields.io/badge/Jolt_Physics-E8552D?style=flat-square) | Véhicules et collisions | 🔜 |
| 🔊 Audio | ![FMOD](https://img.shields.io/badge/FMOD-009FE3?style=flat-square) | Spatialisation et mixage | 🔜 |
| 📊 Profilage | ![Tracy](https://img.shields.io/badge/Tracy-0E9F6E?style=flat-square) | Analyse des performances en temps réel | 🔜 |

<img src="assets/divider.svg" width="100%" alt="">

## 🧱 Architecture

Le moteur est découpé en modules à responsabilité unique :

```
src/engine/
├── core/     platform (SDL) · window · application · log
├── render/   graphics_device · shader · gpu_buffer · gpu_mesh · texture · renderer
├── assets/   fbx_loader (import FBX) · stb_image (décodage PNG) · model_catalog
├── scene/    camera (vol libre) · transform · scene_loader (city.json)
├── world/    world · entity · static_prop · ground
└── editor/   ui (Dear ImGui — éditeur de ville in-game)
```

<img src="assets/architecture.svg" width="100%" alt="Schéma d'architecture de Cinder City">

**Du data au pixel.** Un objet est d'abord une géométrie (`mesh`, sommets +
indices), soit générée à la main (le sol), soit **importée d'un fichier FBX**
via `fbx_loader`. Elle est uploadée en VRAM sous forme de `gpu_mesh` — chargée
**une seule fois** par le `model_catalog` puis partagée par toutes ses instances.
Chaque géométrie est portée par une **entité** (un `transform`, une couleur et un
**matériau**) qui vit dans le **`world`**. Chaque frame, le **`renderer`**
parcourt le monde et dessine chaque entité en sélectionnant le **pipeline
correspondant à son matériau** : `solid_color` pour les objets unis, `grid_floor`
pour le sol quadrillé, et `textured` pour les modèles échantillonnés dans une
**palette** (atlas Synty). Le pipeline texturé lie **deux textures** : l'atlas de
couleur et son **émissif** — le shader les additionne (`base + lueur`), ce qui
allume fenêtres et enseignes là où l'atlas émissif en contient. Un interrupteur
dans `textured.frag` permet de couper la lueur sans toucher au code C++.

**La ville en données, pas en code.** La disposition de la ville ne vit plus dans
le C++ : elle est décrite dans un fichier de scène **`city.json`** (une liste
d'instances `{ modèle, position, rotation, échelle }`) que le `scene_loader` lit
au démarrage pour peupler le monde. Ajouter ou déplacer un bâtiment se fait en
éditant ce fichier — **sans recompiler**. C'est la fondation du futur éditeur.

```json
{ "model": "SM_Gen_Bld_Background_01", "position": [10, 0, 0], "rotation_y": 90, "scale": 1.0 }
```

La scène actuelle en est la preuve : l'**avenue de démonstration** (deux rangées
de façades face à face, espacées selon la largeur réelle de chaque modèle) a été
**générée en données** — 28 instances écrites dans `city.json`, zéro ligne de C++.

**Modèle d'entités (OOP).** Une classe de base `entity` (transform, mesh, couleur,
matériau, `update()`) se spécialise par héritage : `static_prop` (immobile),
bientôt `vehicle`, `pedestrian`… Ajouter un objet au monde tient en une ligne :

```cpp
world_.spawn<static_prop>(catalog_.get("SM_Gen_Bld_Background_01"),
                          transform {.position = {10, 0, 0}},
                          glm::vec4 {1.0f}, material_type::textured);
```

**Éditeur de ville in-game.** On se déplace dans la scène avec une **caméra libre**
(vol au clavier + souris), et on bâtit la ville **directement à la souris** grâce à
une couche **Dear ImGui**. Le picking (rayon curseur → sol) permet de **poser** un
modèle au clic, de **sélectionner** un bâtiment existant, puis de le **déplacer,
tourner, redimensionner ou supprimer** — le tout **sauvegardé** dans `city.json`.
`Tab` bascule entre pilotage caméra et interaction avec l'interface. Le panneau
est organisé en **sections repliables** (Infos · Outil · Modèles · Sélection ·
Scène), avec une **barre de recherche** qui filtre la palette de modèles en direct
et une liste défilante — prêt pour des centaines de modèles.

**Principes de code.** RAII systématique (chaque ressource GPU possédée et
libérée par un objet), C++23 (concepts, `std::format`, designated initializers),
séparation nette entre données (`city.json`), simulation (`world` / `entity`) et
rendu (`renderer`). Le code est **intégralement commenté en français** dans une
optique pédagogique — pour rester maintenable, relisible et prêt au multijoueur.

<img src="assets/divider.svg" width="100%" alt="">

## 📁 Organisation du dépôt

```
cinder-city/
├── src/engine/          le moteur (voir Architecture ci-dessus)
├── shaders/             GLSL source + SPIR-V compilés (.spv)
├── assets/              ⚠ gitignoré — contenus propriétaires (voir ci-dessous)
│   ├── Models/          .fbx triés par catégorie (Buildings, Props, Vehicles…)
│   ├── Textures/        atlas & textures triés (Alts, Emissive, Buildings…)
│   ├── Collision/       meshes de collision (physique, plus tard)
│   └── city.json        la scène — seule donnée versionnée de ce dossier
├── docs/screenshots/    captures datées (AAAA-MM-JJ_description.png)
└── .github/scripts/     outillage (compteur de lignes du README)
```

**Assets propriétaires.** Les modèles et textures proviennent des packs
**Synty POLYGON** (Palm City / Generic), sous licence commerciale : ils ne sont
**pas inclus** dans le dépôt (`assets/` est gitignoré, seule `city.json` est
versionnée). Pour lancer le jeu, il faut posséder ces packs et déposer leurs
`SourceFiles` dans `assets/` — les modèles sont ensuite triés par catégorie et
référencés **par nom** dans `city.json`, jamais par chemin en dur dans le code.

<img src="assets/divider.svg" width="100%" alt="">

## 🚧 Feuille de route

| | Étape | Statut |
| --- | --- | --- |
| ✅ | Socle moteur — fenêtre, boucle applicative, logs | Fait |
| ✅ | Rendu SDL_gpu — device, pipeline, depth buffer | Fait |
| ✅ | Sol 1 km² + caméra perspective | Fait |
| ✅ | Architecture entité/monde (game object OOP) | Fait |
| ✅ | Matériaux par entité — `solid_color` / `grid_floor` | Fait |
| ✅ | Import de modèles FBX (ufbx) + rendu texturé (palette Synty) | Fait |
| ✅ | Ville en données — `city.json` + catalogue de modèles | Fait |
| ✅ | Caméra libre — vol clavier (ZQSD) + souris | Fait |
| ✅ | Intégration Dear ImGui (overlay debug, rendu en 2ᵉ passe) | Fait |
| ✅ | Base de code intégralement commentée (français, pédagogique) | Fait |
| ✅ | Picking souris (rayon curseur → sol) | Fait |
| ✅ | Éditeur in-game — poser / sélectionner / éditer / sauver la ville | Fait |
| ✅ | Assets triés par catégorie + tri des modèles 100 % texturables | Fait |
| ✅ | Rendu émissif — fenêtres/enseignes lumineuses (2 textures additionnées) | Fait |
| ✅ | Avenue de démonstration générée en données (28 façades placées au mètre) | Fait |
| ✅ | Éditeur organisé — sections repliables, recherche, panneau ancré | Fait |
| ⬜ | Multi-matériaux par sous-mesh — chaque partie d'un modèle avec sa texture | À venir |
| ⬜ | Route au sol + surlignage de la sélection | À venir |
| ⬜ | Peupler la ville — véhicules, PNJ | À venir |
| ⬜ | Physique & collisions (Jolt) | À venir |

**Prochaine étape :** 🏙️ Le rendu multi-matériaux, puis peupler la ville.

```
Progression du socle   [■■■■■■■■□□]  80%
```

<img src="assets/divider.svg" width="100%" alt="">

## ⚙️ Construction

**Prérequis**

- Un compilateur **C++23** (Clang 17+, GCC 14+, MSVC 19.4+)
- **CMake ≥ 3.28**
- **`glslc`** pour compiler les shaders (paquet `shaderc`) — sur macOS : `brew install shaderc`

SDL3, GLM, SDL_shadercross, ufbx, stb_image, nlohmann/json et **Dear ImGui**
sont récupérés et compilés automatiquement par CMake (`FetchContent`, versions
épinglées).

**Compiler les shaders** (GLSL → SPIR-V), puis le projet :

```bash
for s in shaders/*.vert shaders/*.frag; do glslc "$s" -o "$s.spv"; done

cmake -S . -B build
cmake --build build -j
```

> ⚠ À refaire après **chaque modification d'un shader** : le moteur charge les
> `.spv`, pas les sources GLSL. Un `.spv` désynchronisé provoque une erreur de
> binding au lancement.

Lance l'exécutable **depuis la racine du projet** — les shaders `.spv` ainsi que
la scène, les modèles et les textures du dossier `assets/` y sont cherchés au
chargement. Sans les packs Synty dans `assets/` (voir plus haut), le programme
s'arrête au chargement de la palette.

**Contrôles**

- Au lancement, la caméra démarre **en haut de l'avenue**, en vue plongeante,
  prête à descendre la rue.
- **Navigation (mode vol) :** `ZQSD` pour se déplacer (scancodes physiques —
  fonctionne aussi en WASD sur QWERTY), la souris pour regarder,
  `Espace` / `Shift` pour monter / descendre.
- **`Tab`** : bascule entre pilotage caméra et interaction avec l'interface
  (en mode vol, la souris est capturée et l'UI ne reçoit aucun clic).
- **Éditeur (mode curseur) :** outil *Placer* → clic gauche pour poser le modèle
  choisi (filtrable via la barre de recherche) ; outil *Sélectionner* → clic sur
  un bâtiment pour l'éditer (déplacer, tourner, redimensionner, supprimer), puis
  *Sauvegarder* — *Recharger* annule les modifications non sauvées.
- **`Échap`** : quitter.

<img src="assets/divider.svg" width="100%" alt="">

## 📜 Licence

Copyright © 2026 **PanDiMou** — **Tous droits réservés.**

Le code source est consultable publiquement à titre de référence uniquement.
Toute utilisation, copie, modification, compilation ou redistribution est
interdite sans autorisation écrite préalable. Voir [`LICENSE`](LICENSE) pour les
termes complets.

<img src="assets/divider.svg" width="100%" alt="">

*Cinder City — bâtie une ligne à la fois.* 🌆
