// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

// ============================================================================
// world — le conteneur de toutes les entités de la scène.
//
// C'est LE propriétaire des objets : il les crée (spawn), les met à jour chaque
// frame (update), et le renderer lui demande la liste pour les dessiner.
// Détruire le world détruit toutes ses entités (grâce à unique_ptr, voir plus bas).
// ============================================================================

#ifndef CINDER_CITY_WORLD_HPP
#define CINDER_CITY_WORLD_HPP

#include "engine/world/entity.hpp"

#include <concepts>   // std::derived_from
#include <memory>     // std::unique_ptr, std::make_unique
#include <utility>    // std::forward, std::move
#include <vector>     // std::vector (tableau dynamique)

namespace cinder {
    class world {
    public:
        // spawn : crée une entité DANS le monde et renvoie une référence dessus.
        //
        // C'est un "template" (patron). Décodage de la ligne suivante :
        //  - T = le type d'entité à créer (ex : static_prop). La contrainte
        //    "std::derived_from<entity>" garantit, à la compilation, que T hérite
        //    bien de `entity` -> impossible de spawn n'importe quoi.
        //  - typename... Args = les arguments à passer au constructeur de T
        //    (nombre et types quelconques), transmis tels quels.
        template <std::derived_from<entity> T, typename... Args>
        T& spawn(Args&&... args) {
            // make_unique crée un T sur le tas et renvoie un unique_ptr (un
            // pointeur "intelligent" qui détruit l'objet tout seul quand il disparaît).
            auto created = std::make_unique<T>(std::forward<Args>(args)...);
            T& reference {*created};                 // on garde une référence pour la renvoyer
            entities_.push_back(std::move(created));  // le monde devient propriétaire (move = transfert)
            return reference;                        // l'appelant peut manipuler l'entité créée
        }

        // Met à jour toutes les entités (leur donne le temps écoulé).
        void update(const float delta_seconds) {
            // "for (... : ...)" = boucle "pour chaque". `entity` est ici un
            // unique_ptr, donc on utilise "->" pour appeler sa méthode update().
            for (const auto& entity : entities_) {
                entity->update(delta_seconds);
            }
        }

        // Donne un accès en lecture à la liste des entités (utilisé par le renderer).
        [[nodiscard]] const std::vector<std::unique_ptr<entity>>& entities() const noexcept {
            return entities_;
        }

        // Vide le monde : détruit toutes les entités. Sert au "Recharger" de l'éditeur.
        // Les unique_ptr détruisent chaque entité automatiquement.
        void clear() {
            entities_.clear();
        }

    private:
        // Un tableau de pointeurs intelligents vers des entités. Pourquoi des
        // pointeurs et pas des entity directement ? Parce qu'on stocke des types
        // FILLES variés (static_prop, plus tard vehicle...) via la classe de base :
        // c'est le polymorphisme, qui nécessite de passer par un pointeur/référence.
        std::vector<std::unique_ptr<entity>> entities_;
    };
}

#endif //CINDER_CITY_WORLD_HPP
