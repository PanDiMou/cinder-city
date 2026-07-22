// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#include "engine/assets/model_catalog.hpp"
#include "engine/assets/fbx_loader.hpp"
#include "engine/render/gpu_mesh.hpp"   // ici on a besoin du type COMPLET de gpu_mesh

#include <filesystem>   // parcours récursif des dossiers (C++17+)
#include <stdexcept>
#include <utility>

namespace cinder {
    namespace {
        // Les modèles sont rangés par catégorie sous "assets/Models/" (Buildings/,
        // Vehicles/, ...). On ne connaît pas la catégorie d'un modèle depuis son seul
        // nom : on cherche donc récursivement le fichier "<nom>.fbx" sous ce dossier.
        // (Fait une seule fois par modèle, car le résultat est ensuite mis en cache.)
        std::string find_model_path(const std::string& name) {
            namespace fs = std::filesystem;
            const std::string filename {name + ".fbx"};
            const fs::path root {"assets/Models"};

            if (fs::exists(root)) {
                for (const fs::directory_entry& entry : fs::recursive_directory_iterator(root)) {
                    if (entry.is_regular_file() && entry.path().filename() == filename) {
                        return entry.path().string();
                    }
                }
            }
            throw std::runtime_error("Modèle introuvable sous assets/Models : " + filename);
        }
    }

    // Constructeur : on mémorise l'adresse du GPU (&device) pour s'en servir plus tard.
    model_catalog::model_catalog(const graphics_device& device)
        : device_ {&device} {}

    // Destructeur : "= default" (rien de spécial à faire), mais il DOIT être dans le
    // .cpp. Raison : détruire les unique_ptr<gpu_mesh> du cache demande de connaître
    // gpu_mesh entièrement, ce qui n'était pas le cas dans le .hpp.
    model_catalog::~model_catalog() = default;

    const gpu_mesh& model_catalog::get(const std::string& name) {
        // Cas 1 : déjà en cache -> on renvoie directement (aucun rechargement).
        // find() cherche la clef ; s'il ne trouve pas, il renvoie end().
        if (const auto found {meshes_.find(name)}; found != meshes_.end()) {
            return *found->second;   // found->second = le unique_ptr ; * = l'objet pointé
        }

        // Cas 2 : première demande -> on construit le chemin, on charge le FBX, on
        // l'uploade en gpu_mesh, et on range le tout dans le cache.
        const std::string path {find_model_path(name)};
        // On charge d'abord le mesh dans une variable : on a besoin de son nom de
        // texture AVANT de le confier au gpu_mesh (qui, lui, ne garde que la géométrie).
        const mesh geometry {load_fbx(path)};
        texture_names_.emplace(name, geometry.texture);   // relève la texture du modèle
        // emplace insère et renvoie une paire {itérateur vers l'élément, bool}.
        // On récupère l'itérateur via un "structured binding" [entry, inserted].
        const auto [entry, inserted] {meshes_.emplace(name, std::make_unique<gpu_mesh>(*device_, geometry))};
        // On renvoie la référence DEPUIS le cache (durée de vie = celle du cache).
        return *entry->second;
    }

    const std::string& model_catalog::texture_name(const std::string& name) const {
        // static : une seule chaîne vide, réutilisée comme valeur de repli. On
        // renvoie une référence, donc elle doit survivre à l'appel (d'où "static").
        static const std::string none {};
        const auto found {texture_names_.find(name)};
        return found != texture_names_.end() ? found->second : none;
    }
}
