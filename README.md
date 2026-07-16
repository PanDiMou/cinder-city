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
éprouvées et pérennes.

| Domaine | Technologie | Rôle |
| --- | --- | --- |
| 🪟 Fenêtrage / entrées | ![SDL3](https://img.shields.io/badge/SDL3-1D4ED8?style=flat-square&logoColor=white) | Fenêtres et périphériques |
| 🎨 Rendu 3D | ![SDL_gpu](https://img.shields.io/badge/SDL__gpu-AC162C?style=flat-square) | Rendu moderne cross-platform (Metal / Vulkan / D3D12) |
| 💥 Physique | ![Jolt](https://img.shields.io/badge/Jolt_Physics-E8552D?style=flat-square) | Véhicules et collisions |
| 🧩 Entités | ![OOP](https://img.shields.io/badge/OOP-6C4AB6?style=flat-square) | Modèle game object maison (transform · entity · world) |
| 📐 Mathématiques | ![GLM](https://img.shields.io/badge/GLM-5586A4?style=flat-square) | Algèbre linéaire 3D |
| 🔊 Audio | ![FMOD](https://img.shields.io/badge/FMOD-009FE3?style=flat-square) | Spatialisation et mixage |
| 🧰 Outils / UI debug | ![Dear ImGui](https://img.shields.io/badge/Dear_ImGui-FF9800?style=flat-square) | Interfaces et outils in-engine |
| 📊 Profilage | ![Tracy](https://img.shields.io/badge/Tracy-0E9F6E?style=flat-square) | Analyse des performances en temps réel |

<img src="assets/divider.svg" width="100%" alt="">

## 🚧 Feuille de route

| | Étape | Statut |
| --- | --- | --- |
| ✅ | Socle moteur — fenêtre, boucle applicative, logs | Fait |
| ✅ | Rendu SDL_gpu — device, pipeline, depth buffer | Fait |
| ✅ | Sol 1 km² + caméra perspective | Fait |
| ✅ | Architecture entité/monde (game object OOP) | Fait |
| 🟨 | Peupler le monde — bâtiments, véhicules, PNJ | En cours |

**Prochaine étape :** 🏙️ Ajouter les premiers objets (bâtiments, véhicules) au monde.

```
Progression du socle   [■■■□□□□□□□]  25%
```

<img src="assets/divider.svg" width="100%" alt="">

## ⚙️ Construction

Projet basé sur **CMake**. Les dépendances sont récupérées et compilées
automatiquement.

```bash
cmake -S . -B build
cmake --build build -j
```

<img src="assets/divider.svg" width="100%" alt="">

## 📜 Licence

Copyright © 2026 **PanDiMou** — **Tous droits réservés.**

Le code source est consultable publiquement à titre de référence uniquement.
Toute utilisation, copie, modification, compilation ou redistribution est
interdite sans autorisation écrite préalable. Voir [`LICENSE`](LICENSE) pour les
termes complets.

<img src="assets/divider.svg" width="100%" alt="">

*Cinder City — bâtie une ligne à la fois.* 🌆
