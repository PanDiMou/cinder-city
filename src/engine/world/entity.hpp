// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

// ============================================================================
// entity — la classe de base de TOUT objet du jeu (le sol, un bâtiment, plus
// tard une voiture, un piéton...).
//
// Idée clé (programmation orientée objet) : on met dans `entity` ce que tous
// les objets ont en commun (une position, une forme à dessiner, une couleur),
// puis on crée des classes spécialisées qui HÉRITENT d'entity (voir static_prop).
//
// Une entité NE possède PAS sa géométrie : elle POINTE vers un gpu_mesh partagé
// (100 bâtiments identiques pointent vers le même mesh -> économie de mémoire).
// ============================================================================

#ifndef CINDER_CITY_ENTITY_HPP
#define CINDER_CITY_ENTITY_HPP

#include "engine/scene/transform.hpp"

namespace cinder {
    class gpu_mesh;   // déclaration anticipée : on manipule juste des pointeurs/références dessus

    // Quel shader (programme GPU) dessine l'entité. Chaque valeur = une "recette" de rendu.
    enum class material_type {
        solid_color, // couleur unie, sans éclairage
        grid_floor,  // sol gris avec un quadrillage procédural
        textured     // échantillonné depuis la texture de palette (modèles importés)
    };

    class entity {
    public:
        // Constructeur. La liste après ":" est la "liste d'initialisation" : elle
        // remplit les membres (transform_, mesh_...) avant l'entrée dans le { }.
        // Note : `mesh_{&mesh}` stocke l'ADRESSE (&) du mesh -> on pointe dessus,
        // on ne le copie pas. Le dernier paramètre a une valeur PAR DÉFAUT
        // (= solid_color) : on peut l'omettre à l'appel.
        entity(const gpu_mesh &mesh, const transform &transform, const glm::vec4 &color, const material_type material = material_type::solid_color)
            : transform_{transform}, mesh_{&mesh}, color_{color}, material_{material} { }

        entity(const entity&) = delete;              // une entité ne se copie pas
        entity& operator=(const entity&) = delete;

        // Destructeur "virtual" : indispensable dès qu'on a de l'héritage, pour que
        // détruire une entity via un pointeur détruise bien la classe fille aussi.
        // "= default" : le compilateur génère le code par défaut.
        virtual ~entity() = default;

        // "virtual" = une classe fille PEUT redéfinir cette méthode (polymorphisme).
        // Ici la version de base ne fait rien. Un `player` la redéfinirait pour bouger.
        // Le nom du paramètre est en commentaire /*...*/ car on ne s'en sert pas ici.
        virtual void update(float /*delta_seconds*/) { }

        // Des "getters" : ils donnent un accès en LECTURE aux membres privés.
        // "const" (méthode) = ne modifie pas l'entité. "const T&" (retour) = on
        // renvoie une référence non modifiable, sans copie. noexcept = ne lève pas d'erreur.
        [[nodiscard]] const transform &get_transform() const noexcept { return transform_; }
        [[nodiscard]] const gpu_mesh &mesh() const noexcept { return *mesh_; } // *mesh_ = "l'objet pointé"
        [[nodiscard]] const glm::vec4 &color() const noexcept { return color_; }
        [[nodiscard]] material_type material() const noexcept { return material_; }

    protected:
        // "protected" = accessible par cette classe ET ses classes filles (mais pas
        // de l'extérieur). Le transform est ici pour qu'un futur `player` puisse le bouger.
        transform transform_;

    private:
        // "private" = accessible uniquement à l'intérieur d'entity.
        const gpu_mesh* mesh_;   // pointeur vers la géométrie partagée (non possédée)
        glm::vec4 color_;        // couleur (r, g, b, a)
        material_type material_; // quelle recette de rendu utiliser
    };
}

#endif //CINDER_CITY_ENTITY_HPP
