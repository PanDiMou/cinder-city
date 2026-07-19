// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

// ============================================================================
// shader — un petit programme qui s'exécute SUR le GPU.
//
// Il en existe deux "étages" (stages) qu'on utilise :
//   - vertex   : s'exécute une fois par sommet (place les points à l'écran).
//   - fragment : s'exécute une fois par pixel (calcule sa couleur).
// (Voir les fichiers .vert / .frag dans shaders/.)
//
// Cette classe charge un shader compilé (fichier .spv) et le prépare pour le GPU.
// ============================================================================

#ifndef CINDER_CITY_SHADER_HPP
#define CINDER_CITY_SHADER_HPP

#include <string>

struct SDL_GPUDevice;   // déclarations anticipées
struct SDL_GPUShader;

namespace cinder {
    class graphics_device;

    // Le type d'étage du shader.
    enum class shader_stage {
        vertex,
        fragment
    };

    class shader {
    public:
        // Charge un shader depuis un fichier .spv, pour l'étage donné (vertex/fragment).
        shader(const graphics_device&, const std::string&, shader_stage);

        shader(const shader&) = delete;              // ressource GPU -> pas de copie
        shader& operator=(const shader&) = delete;

        ~shader();

        // Renvoie le shader SDL brut (le pipeline en a besoin à sa création).
        [[nodiscard]] SDL_GPUShader* native() const noexcept { return shader_; }

    private:
        SDL_GPUDevice* device_ {nullptr};
        SDL_GPUShader* shader_ {nullptr};
    };
}

#endif //CINDER_CITY_SHADER_HPP
