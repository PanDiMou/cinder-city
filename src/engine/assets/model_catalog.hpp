// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

// ============================================================================
// model_catalog — un "cache" de modèles 3D.
//
// Problème : une ville a des centaines de bâtiments, mais souvent le MÊME modèle
// répété. Recharger le fichier FBX à chaque fois gaspillerait mémoire et temps.
// Solution : le catalogue charge chaque modèle UNE seule fois, le garde, et
// renvoie la même géométrie à chaque demande. On dit que c'est un cache "paresseux"
// (lazy) : il ne charge un modèle qu'au moment où on le demande la première fois.
// ============================================================================

#ifndef CINDER_CITY_MODEL_CATALOG_HPP
#define CINDER_CITY_MODEL_CATALOG_HPP

#include <memory>          // std::unique_ptr
#include <string>
#include <unordered_map>   // std::unordered_map = une table clef -> valeur (dictionnaire)

namespace cinder {
    class graphics_device;   // déclarations anticipées (on ne stocke que des pointeurs)
    class gpu_mesh;

    class model_catalog {
    public:
        // "explicit" : évite qu'un graphics_device soit converti tout seul en catalogue.
        explicit model_catalog(const graphics_device&);

        model_catalog(const model_catalog&) = delete;              // pas de copie
        model_catalog& operator=(const model_catalog&) = delete;

        // Le destructeur est seulement DÉCLARÉ ici, et défini dans le .cpp. C'est
        // nécessaire car gpu_mesh n'est ici qu'une déclaration anticipée, et détruire
        // un unique_ptr<gpu_mesh> exige de connaître le type complet (voir le .cpp).
        ~model_catalog();

        // Renvoie la géométrie GPU d'un modèle (par son nom), en la chargeant si
        // c'est la première fois. Renvoie une RÉFÉRENCE : les entités pointeront dessus.
        [[nodiscard]] const gpu_mesh& get(const std::string&);

        // Renvoie le NOM de texture (sans extension) que le FBX du modèle référence,
        // ou une chaîne vide s'il n'en désigne aucune. À appeler APRÈS get() (c'est
        // lui qui remplit l'info au chargement).
        [[nodiscard]] const std::string& texture_name(const std::string&) const;

    private:
        // Pointeur vers le GPU (non possédé) : sert à uploader les modèles chargés.
        const graphics_device* device_;

        // Le cache : chaque nom de modèle -> son gpu_mesh (dans un unique_ptr pour
        // que l'adresse reste stable et que la mémoire soit libérée automatiquement).
        std::unordered_map<std::string, std::unique_ptr<gpu_mesh>> meshes_;

        // En parallèle : chaque nom de modèle -> le nom de sa texture de couleur,
        // relevé dans le FBX au moment du chargement.
        std::unordered_map<std::string, std::string> texture_names_;
    };
}

#endif //CINDER_CITY_MODEL_CATALOG_HPP
