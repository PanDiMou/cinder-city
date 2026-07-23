// Cinder City — Copyright (c) 2026 PanDiMou. Tous droits réservés.
// Propriétaire, Consultation autorisée.
// Utilisation, copie, modification et distribution interdites sans autorisation écrite.
// Voir le fichier LICENSE à la racine du dépôt.

#include "engine/assets/fbx_loader.hpp"
#include <ufbx.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <span>
#include <string_view>

namespace cinder {
    namespace {
        // Nom de fichier seul, sans dossier ni extension : "dir/Road_01.fbx" -> "Road_01".
        [[nodiscard]] std::string stem(std::string_view path) {
            path.remove_prefix(path.find_last_of("/\\") + 1);

            return std::string{
                path.substr(0, path.rfind('.'))
            };
        }
    }

    mesh load_fbx(const std::string& path) {
        // Axes-Y vers le haut, 1 unité = 1 mètre, la conversion est portée par une transformation racine.
        ufbx_load_opts opts {};
        opts.target_axes = ufbx_axes_right_handed_y_up;
        opts.target_unit_meters = 1.0f;
        opts.space_conversion = UFBX_SPACE_CONVERSION_TRANSFORM_ROOT;

        ufbx_error error;
        // On charge le fichier. L'unique_ptr (deleter ufbx_free_scene) devient propriétaire
        // de la scène : elle sera libérée toute seule en sortie de fonction.
        const std::unique_ptr<ufbx_scene, decltype(&ufbx_free_scene)> scene {
            ufbx_load_file(path.c_str(), &opts, &error), &ufbx_free_scene
        };
        if (scene == nullptr) {
            throw std::runtime_error("load_fbx(" + path + ") : " + error.description.data);
        }

        mesh out_mesh;

        // On parcourt TOUS les nœuds de la scène.
        // Un modèle FBX est souvent découpé en morceaux (murs, roues, portes…), on va tous les fusionner en une seule géométrie.
        for (const ufbx_node* node : scene->nodes) {
            if (node->is_root || node->mesh == nullptr) {
                continue;
            }

            // Le maillage porté par ce nœud : UNE pièce du modèle (un mur, une roue…) qu'on
            // va trianguler puis fusionner dans "out_mesh".
            const ufbx_mesh* part {node->mesh};

            // Tampon d'indices pour la triangulation, taille = plus grande face (triangles × 3),
            // alloué une fois et réutilisé pour chaque face.
            std::vector<std::uint32_t> triangle_indices(part->max_face_triangles * 3);

            // Pour chaque face de la pièce (ufbx_face = petit descripteur : où commence la face + nombre de sommets).
            for (const ufbx_face face : part->faces) {
                // Découpe la face en triangle.
                const std::uint32_t triangles{
                    ufbx_triangulate_face(triangle_indices.data(), triangle_indices.size(), part, face)
                };

                // Pour chaque sommet des triangles (les `triangles * 3` premiers indices du tampon).
                for (const std::uint32_t index : std::span{triangle_indices}.first(triangles * 3)) {
                    // Position du sommet dans la pièce (repère local)...
                    const ufbx_vec3 local {ufbx_get_vertex_vec3(&part->vertex_position, index)};
                    // ... « cuite » en position monde : geometry_to_world intègre placement + conversion Y-up/mètres.
                    const ufbx_vec3 world {ufbx_transform_position(&node->geometry_to_world, local)};

                    // UV du sommet si le maillage en a sinon repli : on a
                    // ainsi toujours un UV valide, sans cas particulier plus loin.
                    const ufbx_vec2 uv {
                        part->vertex_uv.exists ? ufbx_get_vertex_vec2(&part->vertex_uv, index) : ufbx_vec2{0.0, 0.0}
                    };

                    // On empile le sommet.
                    // Aucune optimisation : chaque sommet a son propre indice,
                    // qui suit l'ordre d'ajout (0,1,2,…) -> pas de sommets partagés.
                    out_mesh.indices.push_back(static_cast<std::uint32_t>(out_mesh.vertices.size()));
                    out_mesh.vertices.push_back(vertex {
                        // Double (ufbx) → float (notre vertex). Flip V : UV FBX partent d'en bas,
                        // nos textures d'en haut, d'où le 1 - v.
                        .position = {static_cast<float>(world.x), static_cast<float>(world.y), static_cast<float>(world.z)},
                        .uv = {static_cast<float>(uv.x), 1.0f - static_cast<float>(uv.y)}
                    });
                }
            }
        }

        // Nom de la texture de couleur : la première trouvée (canal diffuse, sinon base_color).
        for (const ufbx_material* material : scene->materials) {
            const ufbx_texture* tex {material->fbx.diffuse_color.texture};
            if (tex == nullptr) {
                tex = material->pbr.base_color.texture;
            }

            if (tex == nullptr) {
                continue;
            }

            const auto&[data, length] {tex->relative_filename.length > 0 ? tex->relative_filename : tex->filename};
            out_mesh.texture = stem(data);
            break;
        }

        // Rien collecté = fichier sans géométrie exploitable (vide, ou juste lumières/caméras).
        // On échoue tôt et clairement plutôt que de renvoyer un mesh vide.
        if (out_mesh.vertices.empty()) {
            throw std::runtime_error("load_fbx(" + path + ") : aucune géométrie trouvée");
        }
        return out_mesh;
    }
}
