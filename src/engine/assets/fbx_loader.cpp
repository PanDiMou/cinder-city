// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire et confidentiel. Consultation autorisée ; utilisation, copie,
// modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#include "engine/assets/fbx_loader.hpp"

#include <ufbx.h>       // la bibliothèque de lecture FBX

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace cinder {
    mesh load_fbx(const std::string& path) {
        // 1) On règle les OPTIONS de chargement pour qu'ufbx nous rende le modèle
        //    directement dans NOS conventions (évite de convertir nous-mêmes).
        ufbx_load_opts opts {};
        opts.target_axes = ufbx_axes_right_handed_y_up;             // Y vers le haut
        opts.target_unit_meters = 1.0f;                            // 1 unité = 1 mètre
        opts.space_conversion = UFBX_SPACE_CONVERSION_TRANSFORM_ROOT;

        // 2) On charge le fichier. ufbx renvoie une "scène" (arbre d'objets) ou
        //    nullptr en cas d'échec, avec la raison dans `error`.
        ufbx_error error;
        ufbx_scene* scene {ufbx_load_file(path.c_str(), &opts, &error)};
        if (scene == nullptr) {
            throw std::runtime_error("load_fbx(" + path + ") : " + error.description.data);
        }

        mesh out;   // la géométrie qu'on va remplir et renvoyer

        // 3) On parcourt tous les "nœuds" de la scène. Un modèle FBX peut être
        //    découpé en plusieurs morceaux (murs, toit...) ; on les fusionne tous
        //    en une seule géométrie, car nos bâtiments ne bougent pas.
        for (std::size_t node_index {0}; node_index < scene->nodes.count; ++node_index) {
            const ufbx_node* node {scene->nodes.data[node_index]};
            // On saute le nœud racine et les nœuds sans géométrie (ex : une lumière).
            if (node->is_root || node->mesh == nullptr) {
                continue;   // "continue" = passe directement à l'itération suivante
            }

            const ufbx_mesh* part {node->mesh};

            // Tampon réutilisable pour la triangulation, dimensionné pour la plus
            // grande face de cette pièce (x3 car un triangle = 3 sommets).
            std::vector<std::uint32_t> triangle_indices(part->max_face_triangles * 3);

            // 4) Pour chaque face du morceau...
            for (std::size_t face_index {0}; face_index < part->faces.count; ++face_index) {
                const ufbx_face face {part->faces.data[face_index]};

                // ...on la découpe en triangles (le GPU ne sait dessiner que ça).
                // ufbx écrit les indices dans triangle_indices et renvoie leur nombre.
                const std::uint32_t triangles {
                    ufbx_triangulate_face(triangle_indices.data(), triangle_indices.size(), part, face)
                };

                // 5) Pour chaque sommet des triangles obtenus...
                for (std::size_t i {0}; i < static_cast<std::size_t>(triangles) * 3; ++i) {
                    const std::uint32_t index {triangle_indices[i]};
                    // Position du sommet dans le repère local du morceau...
                    const ufbx_vec3 local {ufbx_get_vertex_vec3(&part->vertex_position, index)};
                    // ...puis convertie en position MONDE (on "cuit" la transformation
                    // du nœud dans les sommets, puisque l'objet ne bougera plus).
                    const ufbx_vec3 world {ufbx_transform_position(&node->geometry_to_world, local)};

                    // La coordonnée de texture (UV). Le "? :" est un ternaire :
                    // si le mesh a des UV on les lit, sinon on met {0, 0}.
                    const ufbx_vec2 uv {part->vertex_uv.exists
                        ? ufbx_get_vertex_vec2(&part->vertex_uv, index)
                        : ufbx_vec2{0.0, 0.0}};

                    // On ajoute le sommet à notre géométrie. On n'optimise pas :
                    // chaque triangle a ses 3 sommets, l'indice suit simplement l'ordre.
                    out.indices.push_back(static_cast<std::uint32_t>(out.vertices.size()));
                    out.vertices.push_back(vertex {
                        // static_cast<float> : ufbx travaille en double (double précision),
                        // notre vertex en float -> on convertit.
                        .position = {static_cast<float>(world.x),
                                     static_cast<float>(world.y),
                                     static_cast<float>(world.z)},
                        // Flip V : les UV FBX partent d'en bas, nos textures d'en haut,
                        // d'où le "1 - v" pour inverser l'axe vertical.
                        .uv = {static_cast<float>(uv.x),
                               1.0f - static_cast<float>(uv.y)}
                    });
                }
            }
        }

        // 6) On libère la scène ufbx : on a déjà copié ce qu'il nous fallait.
        ufbx_free_scene(scene);

        // Garde-fou : si le fichier ne contenait aucune géométrie, c'est une erreur.
        if (out.vertices.empty()) {
            throw std::runtime_error("load_fbx(" + path + ") : aucune géométrie trouvée");
        }
        return out;
    }
}
