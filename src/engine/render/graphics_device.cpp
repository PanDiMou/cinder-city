// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

#include "engine/render/graphics_device.hpp"
#include "engine/core/window.hpp"

#include <SDL3/SDL.h>
#include <SDL3_shadercross/SDL_shadercross.h>
#include <stdexcept>
#include <string>

// "Compilation conditionnelle" : le compilateur définit NDEBUG en mode Release.
// On active donc la validation du GPU (des vérifications utiles mais coûteuses)
// uniquement en Debug, pour attraper nos erreurs pendant le développement.
#ifdef NDEBUG
    constexpr bool debug_mode {false};   // build Release : pas de validation
#else
    constexpr bool debug_mode {true};    // build Debug : validation activée
#endif

namespace cinder {
    graphics_device::graphics_device(window& window): window_ {window.native()} {
        // 1) Crée le GPU. On lui dit quels formats de shaders on saura lui fournir :
        // SPIRV (notre format de base), MSL (Metal, pour Mac), DXIL (Direct3D, Windows).
        // SDL choisira le bon selon la plateforme.
        device_ = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV
            | SDL_GPU_SHADERFORMAT_MSL
            | SDL_GPU_SHADERFORMAT_DXIL, debug_mode, nullptr);

        if (device_ == nullptr) {
            throw std::runtime_error(std::string{"Échec de SDL_CreateGPUDevice : "} + SDL_GetError());
        }

        // 2) Associe le GPU à notre fenêtre (pour pouvoir dessiner dedans). En cas
        // d'échec, on détruit le GPU déjà créé avant de lever l'erreur (nettoyage manuel,
        // car l'objet n'est pas encore complètement construit).
        if (!SDL_ClaimWindowForGPUDevice(device_, window_)) {
            SDL_DestroyGPUDevice(device_);
            throw std::runtime_error(std::string{"Échec de SDL_ClaimWindowForGPUDevice : "} + SDL_GetError());
        }

        // 3) Initialise SDL_shadercross, qui traduira nos shaders SPIR-V vers le
        // format natif du GPU au moment de leur chargement.
        if (!SDL_ShaderCross_Init()) {
            SDL_ReleaseWindowFromGPUDevice(device_, window_);
            SDL_DestroyGPUDevice(device_);
            throw std::runtime_error(std::string{"Échec de SDL_ShaderCross_Init : "} + SDL_GetError());
        }
    }

    // Destructeur : on démonte dans l'ordre inverse de la construction.
    graphics_device::~graphics_device() {
        SDL_ShaderCross_Quit();
        if (device_ != nullptr) {
            SDL_ReleaseWindowFromGPUDevice(device_, window_);
            SDL_DestroyGPUDevice(device_);
        }
    }
}
