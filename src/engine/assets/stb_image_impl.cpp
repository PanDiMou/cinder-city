// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// stb_image_impl.cpp — le "corps" de la bibliothèque stb_image (décodage d'images).
//
// stb_image est "header-only" : tout son code tient dans stb_image.h, mais ce
// code n'est réellement COMPILÉ que dans le fichier qui définit la macro
// STB_IMAGE_IMPLEMENTATION avant de l'inclure. C'est ce que fait ce fichier, et
// LUI SEUL. Partout ailleurs, on écrit juste #include <stb_image.h> (déclarations).
//
// Pourquoi un seul endroit ? Si deux fichiers compilaient le code, l'éditeur de
// liens verrait la même fonction définie deux fois -> erreur "multiple definition".
// ============================================================================

#define STB_IMAGE_IMPLEMENTATION   // "active" le vrai code dans le header ci-dessous
#include <stb_image.h>
