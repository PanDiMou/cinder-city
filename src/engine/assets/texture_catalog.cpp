// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#include "engine/assets/texture_catalog.hpp"
#include "engine/render/texture.hpp"    // ici on a besoin du type COMPLET de texture

#include <filesystem>   // parcours récursif des dossiers (C++17+)
#include <stdexcept>

namespace cinder {
    namespace {
        // Certains modèles Synty référencent un atlas sous un nom de pack DIFFÉRENT
        // de celui qu'on possède, alors que l'image est en réalité la même (même
        // agencement d'UV). On "traduit" donc ces noms vers l'atlas équivalent
        // présent dans nos assets, avant de chercher le fichier.
        //   - "PolygonSunshineCity_Texture_XX" est l'ancien "PolygonPalmCity_XX"
        //     (ex : la carrosserie des véhicules) -> on mappe vers PalmCity.
        std::string resolve_alias(const std::string& name) {
            const std::string prefix {"PolygonSunshineCity_Texture_"};
            if (name.rfind(prefix, 0) == 0) {   // le nom COMMENCE par le préfixe ?
                return "PolygonPalmCity_" + name.substr(prefix.size());
            }
            return name;   // aucun alias connu -> on garde le nom tel quel
        }

        // Les textures sont rangées par sous-dossiers sous "assets/Textures/" (Alts/,
        // Emissive/, Buildings/...). On ne connaît pas le sous-dossier d'une image
        // depuis son seul nom : on cherche donc récursivement "<nom>.<ext>" sous ce
        // dossier. (Fait une seule fois par image, car le résultat est mis en cache.)
        std::string find_texture_path(const std::string& raw_name) {
            namespace fs = std::filesystem;
            const fs::path root {"assets/Textures"};

            // On traduit d'abord les alias connus (pack équivalent).
            const std::string name {resolve_alias(raw_name)};

            // On accepte le nom avec OU sans extension : si "name" contient déjà un
            // point, on le cherche tel quel ; sinon on essaie .png puis .tga.
            const bool has_ext {name.find('.') != std::string::npos};

            if (fs::exists(root)) {
                for (const fs::directory_entry& entry : fs::recursive_directory_iterator(root)) {
                    if (!entry.is_regular_file()) {
                        continue;
                    }
                    const std::string file {entry.path().filename().string()};
                    if (has_ext) {
                        if (file == name) { return entry.path().string(); }
                    } else if (file == name + ".png" || file == name + ".tga") {
                        return entry.path().string();
                    }
                }
            }
            throw std::runtime_error("Texture introuvable sous assets/Textures : " + name);
        }
    }

    // Constructeur : on mémorise l'adresse du GPU pour s'en servir aux chargements.
    texture_catalog::texture_catalog(const graphics_device& device)
        : device_ {&device} {}

    // Destructeur "= default", mais DANS le .cpp : détruire les unique_ptr<texture>
    // du cache exige de connaître texture entièrement (inclus ici, pas dans le .hpp).
    texture_catalog::~texture_catalog() = default;

    const texture& texture_catalog::get(const std::string& name) {
        // Cas 1 : déjà en cache -> on renvoie directement (aucun rechargement).
        if (const auto found {textures_.find(name)}; found != textures_.end()) {
            return *found->second;
        }

        // Cas 2 : première demande -> on résout le chemin, on charge l'image en VRAM,
        // et on range la texture dans le cache. emplace renvoie {itérateur, bool}.
        const std::string path {find_texture_path(name)};
        const auto [entry, inserted] {textures_.emplace(name, std::make_unique<texture>(*device_, path))};
        return *entry->second;   // référence stable (durée de vie = celle du cache)
    }
}
