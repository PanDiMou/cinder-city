// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#include "engine/assets/model_catalog.hpp"
#include "engine/assets/fbx_loader.hpp"
#include "engine/render/gpu_mesh.hpp"   // ici on a besoin du type COMPLET de gpu_mesh

#include <utility>

namespace cinder {
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
        const std::string path {"assets/models/" + name + ".fbx"};
        // emplace insère et renvoie une paire {itérateur vers l'élément, bool}.
        // On récupère l'itérateur via un "structured binding" [entry, inserted].
        const auto [entry, inserted] {meshes_.emplace(name, std::make_unique<gpu_mesh>(*device_, load_fbx(path)))};
        // On renvoie la référence DEPUIS le cache (durée de vie = celle du cache).
        return *entry->second;
    }
}
