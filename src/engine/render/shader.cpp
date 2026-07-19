// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#include "engine/render/shader.hpp"
#include "engine/render/graphics_device.hpp"

#include <SDL3/SDL.h>
#include <SDL3_shadercross/SDL_shadercross.h>

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace cinder {
    // Un namespace anonyme "{ ... }" rend son contenu PRIVÉ à ce fichier .cpp :
    // read_file n'est visible/appelable que d'ici. C'est le pendant C++ du "static".
    namespace {
        // Lit tout le contenu d'un fichier binaire et le renvoie sous forme d'octets.
        std::vector<std::uint8_t> read_file(const std::string& path) {
            // "binary" : lecture octet par octet (pas de conversion de texte).
            // "ate" : on se place directement à la FIN pour connaître la taille.
            std::ifstream file {path, std::ios::binary | std::ios::ate};
            if (!file) {
                throw std::runtime_error("Shader introuvable : " + path);
            }
            const auto size {static_cast<std::size_t>(file.tellg())};   // tellg() = position = taille
            std::vector<std::uint8_t> bytes(size);
            file.seekg(0);   // on revient au début pour tout lire
            // reinterpret_cast : file.read veut un char*, on lui donne l'adresse de
            // nos octets réinterprétée comme telle. Cast "bas niveau", à utiliser avec soin.
            file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(size));
            return bytes;
        }
    }

    shader::shader(const graphics_device& device, const std::string& spirv_path, const shader_stage stage)
        : device_ {device.native()} {

        // 1) On lit le fichier compilé (.spv), qui contient du "bytecode" SPIR-V.
        const std::vector bytecode {read_file(spirv_path)};

        // 2) On décrit le shader à shadercross : ses octets, son point d'entrée
        // ("main", la fonction principale du shader), et son étage.
        SDL_ShaderCross_SPIRV_Info info {};
        info.bytecode = bytecode.data();
        info.bytecode_size = bytecode.size();
        info.entrypoint = "main";
        info.shader_stage = stage == shader_stage::vertex
            ? SDL_SHADERCROSS_SHADERSTAGE_VERTEX
            : SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT;

        // 3) La "réflexion" analyse le shader pour détecter tout seul les ressources
        // qu'il utilise (uniform buffers, textures...). Pratique : pas à les déclarer à la main.
        SDL_ShaderCross_GraphicsShaderMetadata* metadata {
            SDL_ShaderCross_ReflectGraphicsSPIRV(bytecode.data(), bytecode.size(), 0)
        };
        if (metadata == nullptr) {
            throw std::runtime_error("Échec de SDL_ShaderCross_ReflectGraphicsSPIRV : " + std::string{SDL_GetError()});
        }

        // 4) On traduit le SPIR-V vers le format natif du GPU (ex : Metal sur Mac)
        // et on crée le shader final.
        shader_ = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(
            device_, &info, &metadata->resource_info, 0);
        SDL_free(metadata);   // metadata a été alloué par SDL : on le libère (SDL_free, pas delete)

        if (shader_ == nullptr) {
            throw std::runtime_error("Échec de SDL_ShaderCross_CompileGraphicsShaderFromSPIRV : " + std::string{SDL_GetError()});
        }
    }

    shader::~shader() {
        if (shader_ != nullptr) {
            SDL_ReleaseGPUShader(device_, shader_);
        }
    }
}
