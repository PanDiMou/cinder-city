<img src="assets/banner.svg" width="100%" alt="Cinder City">

### *Un monde ouvert. Un développeur. Une obsession.*

Un jeu vidéo en monde ouvert urbain, construit intégralement à la main —
sans moteur du commerce, une ligne de code après l'autre.

![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![SDL_gpu](https://img.shields.io/badge/SDL__gpu-rendu-AC162C?style=for-the-badge)
![SDL3](https://img.shields.io/badge/SDL-3.x-1D4ED8?style=for-the-badge)
![CMake](https://img.shields.io/badge/CMake-build-064F8C?style=for-the-badge&logo=cmake&logoColor=white)

![Status](https://img.shields.io/badge/statut-en%20développement-F5A623?style=flat-square)
![Solo Dev](https://img.shields.io/badge/équipe-1%20développeur-FF6B35?style=flat-square)
![Map](https://img.shields.io/badge/monde-1%20km²-2EA043?style=flat-square)
![License](https://img.shields.io/badge/licence-tous%20droits%20réservés-D63A2F?style=flat-square)

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
| 🧩 Entités | ![OOP](https://img.shields.io/badge/OOP-6C4AB6?style=flat-square) | Modèle game object maison (transform · entity · world) | ✅ |
| 💥 Physique | ![Jolt](https://img.shields.io/badge/Jolt_Physics-E8552D?style=flat-square) | Véhicules et collisions | 🔜 |
| 🔊 Audio | ![FMOD](https://img.shields.io/badge/FMOD-009FE3?style=flat-square) | Spatialisation et mixage | 🔜 |
| 🧰 Outils / UI debug | ![Dear ImGui](https://img.shields.io/badge/Dear_ImGui-FF9800?style=flat-square) | Interfaces et outils in-engine | 🔜 |
| 📊 Profilage | ![Tracy](https://img.shields.io/badge/Tracy-0E9F6E?style=flat-square) | Analyse des performances en temps réel | 🔜 |

<img src="assets/divider.svg" width="100%" alt="">

## 🧱 Architecture

Le moteur est découpé en modules à responsabilité unique :

```
src/engine/
├── core/     platform (SDL) · window · application · log
├── render/   graphics_device · shader · gpu_buffer · gpu_mesh · renderer
├── scene/    camera · transform (position / rotation / échelle)
└── world/    world · entity · static_prop · player · ground · cube
```

<img src="assets/architecture.svg" width="100%" alt="Schéma d'architecture de Cinder City">

**Du data au pixel.** Un objet est d'abord une géométrie (`mesh`, sommets +
indices), uploadée en VRAM sous forme de `gpu_mesh`. Il est porté par une
**entité** (un `transform`, une couleur et un **matériau**) qui vit dans le
**`world`**. Chaque frame, le **`renderer`** parcourt le monde et dessine chaque
entité avec sa matrice caméra et sa couleur, en sélectionnant le **pipeline
correspondant à son matériau** (`solid_color` pour les objets, `grid_floor` pour
le sol quadrillé).

**Modèle d'entités (OOP).** Une classe de base `entity` (transform, mesh,
couleur, `update()`) se spécialise par héritage : `static_prop` (immobile),
`player` (piloté aux flèches), bientôt `vehicle`, `pedestrian`… Ajouter un objet
au monde tient en une ligne :

```cpp
world_.spawn<static_prop>(cube_mesh_, transform {.position = {0, 0.5f, 0}},
                          glm::vec4 {1.0f, 0.5f, 0.0f, 1.0f}); // cube orange
```

**Principes de code.** RAII systématique (chaque ressource GPU possédée et
libérée par un objet), C++23 (`std::expected`-friendly, concepts, `std::format`),
séparation nette entre simulation (`world` / `entity`) et rendu (`renderer`) —
pour rester maintenable et prêt au multijoueur.

<img src="assets/divider.svg" width="100%" alt="">

## 🚧 Feuille de route

| | Étape | Statut |
| --- | --- | --- |
| ✅ | Socle moteur — fenêtre, boucle applicative, logs | Fait |
| ✅ | Rendu SDL_gpu — device, pipeline, depth buffer | Fait |
| ✅ | Sol 1 km² + caméra perspective | Fait |
| ✅ | Architecture entité/monde (game object OOP) | Fait |
| ✅ | Couleur par entité + premier objet (cube orange) | Fait |
| ✅ | Contrôle clavier — déplacer un objet aux flèches | Fait |
| ✅ | Caméra qui suit le joueur (chase cam) | Fait |
| ✅ | Matériaux par entité + sol gris quadrillé (`solid_color` / `grid_floor`) | Fait |
| 🟨 | Peupler le monde — bâtiments, véhicules, PNJ | En cours |
| ⬜ | Caméra orbitale / relative (souris) | À venir |
| ⬜ | Physique & collisions (Jolt) | À venir |

**Prochaine étape :** 🏙️ Poser les premiers bâtiments sur le sol.

```
Progression du socle   [■■■■■□□□□□]  45%
```

<img src="assets/divider.svg" width="100%" alt="">

## ⚙️ Construction

**Prérequis**

- Un compilateur **C++23** (Clang 17+, GCC 14+, MSVC 19.4+)
- **CMake ≥ 3.28**
- **`glslc`** pour compiler les shaders (paquet `shaderc`) — sur macOS : `brew install shaderc`

SDL3, GLM et SDL_shadercross sont récupérés et compilés automatiquement par CMake.

**Compiler les shaders** (GLSL → SPIR-V), puis le projet :

```bash
glslc shaders/solid_color.vert -o shaders/solid_color.vert.spv
glslc shaders/solid_color.frag -o shaders/solid_color.frag.spv
glslc shaders/grid_floor.vert  -o shaders/grid_floor.vert.spv
glslc shaders/grid_floor.frag  -o shaders/grid_floor.frag.spv

cmake -S . -B build
cmake --build build -j
```

Lance l'exécutable **depuis la racine du projet** — les shaders `.spv` y sont
cherchés au chargement.

<img src="assets/divider.svg" width="100%" alt="">

## 📜 Licence

Copyright © 2026 **PanDiMou** — **Tous droits réservés.**

Le code source est consultable publiquement à titre de référence uniquement.
Toute utilisation, copie, modification, compilation ou redistribution est
interdite sans autorisation écrite préalable. Voir [`LICENSE`](LICENSE) pour les
termes complets.

<img src="assets/divider.svg" width="100%" alt="">

*Cinder City — bâtie une ligne à la fois.* 🌆
