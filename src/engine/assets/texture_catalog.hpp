// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

// ============================================================================
// texture_catalog — un "cache" de textures, jumeau de model_catalog.
//
// Problème : plein de modèles partagent la MÊME image (toutes les routes Generic
// utilisent Generic_Road_01.png). La charger en VRAM une fois par modèle serait
// du gaspillage. Solution : le catalogue charge chaque image UNE seule fois, la
// garde, et renvoie la même texture à chaque demande (cache "paresseux").
//
// On demande une texture par son NOM de fichier (ex : "Generic_Road_01") ; le
// catalogue la retrouve tout seul sous assets/Textures (recherche récursive).
// ============================================================================

#ifndef CINDER_CITY_TEXTURE_CATALOG_HPP
#define CINDER_CITY_TEXTURE_CATALOG_HPP

#include <memory>          // std::unique_ptr
#include <string>
#include <unordered_map>   // table clef -> valeur (dictionnaire)

namespace cinder {
    class graphics_device;   // déclarations anticipées (on ne stocke que des pointeurs)
    class texture;

    class texture_catalog {
    public:
        // "explicit" : évite qu'un graphics_device soit converti tout seul en catalogue.
        explicit texture_catalog(const graphics_device&);

        texture_catalog(const texture_catalog&) = delete;              // pas de copie
        texture_catalog& operator=(const texture_catalog&) = delete;

        // Déclaré ici, défini dans le .cpp : détruire un unique_ptr<texture> exige
        // le type complet de texture, qui n'est ici qu'une déclaration anticipée.
        ~texture_catalog();

        // Renvoie la texture correspondant à un nom de fichier (sans extension, ex
        // "Generic_Road_01"), en la chargeant si c'est la première fois. Renvoie une
        // RÉFÉRENCE stable : le renderer pointera dessus au moment du dessin.
        [[nodiscard]] const texture& get(const std::string& name);

    private:
        // Pointeur vers le GPU (non possédé) : sert à uploader les images chargées.
        const graphics_device* device_;

        // Le cache : chaque nom de fichier -> sa texture (unique_ptr pour une adresse
        // stable et une libération automatique).
        std::unordered_map<std::string, std::unique_ptr<texture>> textures_;
    };
}

#endif //CINDER_CITY_TEXTURE_CATALOG_HPP
