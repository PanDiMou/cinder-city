// Cinder City — Copyright (c) 2026 PanDiMou. All rights reserved.
// Proprietary and confidential. Viewing permitted; use, copying,
// modification, and distribution prohibited without written permission.
// See LICENSE at the repository root.

// ============================================================================
// camera — le point de vue par lequel on regarde le monde 3D.
//
// Son rôle : fournir la matrice "vue-projection" qui transforme les objets 3D
// en image 2D à l'écran. Ici, une caméra "vol libre" : la souris oriente le
// regard (look), le clavier déplace (move). Le renderer lui demande sa matrice
// à chaque frame.
//
// Deux angles décrivent l'orientation :
//   - yaw   = rotation gauche/droite (comme tourner la tête horizontalement)
//   - pitch = rotation haut/bas (lever/baisser la tête)
// ============================================================================

#ifndef CINDER_CITY_CAMERA_HPP
#define CINDER_CITY_CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>     // std::cos, std::sin, std::abs
#include <optional>  // std::optional (une valeur "peut-être présente")

namespace cinder {
    class camera {
    public:
        // Oriente la caméra. Les deltas viennent du déplacement de la souris.
        void look(const float delta_yaw, const float delta_pitch) {
            yaw_ += delta_yaw;
            // glm::clamp "coince" pitch_ entre min et max : on ne peut pas
            // regarder plus haut/bas que la verticale, sinon l'image se retourne.
            pitch_ = glm::clamp(pitch_ + delta_pitch, min_pitch_, max_pitch_);
        }

        // Déplace la caméra. `local` est une direction dans le repère de la caméra
        // (x = droite, y = haut, z = avant). delta_seconds sert à rendre la vitesse
        // indépendante du framerate.
        void move(const glm::vec3& local, const float delta_seconds) {
            const glm::vec3 forward {direction()};                          // vers où on regarde
            // Le "produit vectoriel" (cross) de deux vecteurs en donne un 3e,
            // perpendiculaire aux deux : ici "avant" x "haut" = "droite".
            const glm::vec3 right {glm::normalize(glm::cross(forward, up_))};
            // Position finale = on avance le long de droite/haut/avant selon `local`,
            // le tout dosé par la vitesse et le temps écoulé.
            position_ += (right * local.x + up_ * local.y + forward * local.z) * speed_ * delta_seconds;
        }

        // Renvoie la matrice qui projette le monde 3D sur l'écran, pour un ratio
        // largeur/hauteur donné (aspect). C'est ce que le shader multiplie à chaque sommet.
        [[nodiscard]] glm::mat4 view_projection(const float aspect) const {
            // "view" : positionne le monde par rapport à la caméra (où elle est,
            // ce qu'elle regarde : position_ + direction()).
            const glm::mat4 view {glm::lookAt(position_, position_ + direction(), up_)};
            // "projection" : simule la perspective (les objets lointains rapetissent).
            // fov = champ de vision, near/far = distances de rendu min/max.
            const glm::mat4 projection {glm::perspective(fov_, aspect, near_, far_)};
            return projection * view;   // combinées en une seule matrice
        }

        // PICKING : renvoie le point du SOL (plan Y=0) visé par la souris.
        // Renvoie std::optional : soit un point, soit "rien" (nullopt) si le rayon
        // ne touche pas le sol. mouse = position du curseur en pixels ; viewport =
        // taille de la fenêtre en pixels.
        [[nodiscard]] std::optional<glm::vec3> pick_ground(const glm::vec2 mouse, const glm::vec2 viewport) const {
            // 1) Pixels -> NDC (coordonnées normalisées de -1 à 1). On inverse Y car
            //    l'écran compte de haut en bas, alors que le NDC va de bas en haut.
            const float ndc_x {2.0f * mouse.x / viewport.x - 1.0f};
            const float ndc_y {1.0f - 2.0f * mouse.y / viewport.y};

            // 2) On inverse la matrice caméra : elle projette le monde vers l'écran,
            //    donc son inverse fait le trajet retour (écran -> monde).
            const float aspect {viewport.x / viewport.y};
            const glm::mat4 inverse_vp {glm::inverse(view_projection(aspect))};

            // 3) On reconstruit le rayon : deux points, l'un sur le plan proche (z=0),
            //    l'autre sur le plan lointain (z=1). La division par w ("division
            //    perspective") ramène ces vec4 en vraies coordonnées 3D.
            glm::vec4 near_point {inverse_vp * glm::vec4{ndc_x, ndc_y, 0.0f, 1.0f}};
            glm::vec4 far_point  {inverse_vp * glm::vec4{ndc_x, ndc_y, 1.0f, 1.0f}};
            near_point /= near_point.w;
            far_point  /= far_point.w;

            const glm::vec3 origin {near_point};
            const glm::vec3 ray {glm::normalize(glm::vec3{far_point} - glm::vec3{near_point})};

            // 4) Intersection rayon / sol (plan Y=0). On cherche la distance t telle
            //    que origin.y + t * ray.y = 0. Si ray.y ~ 0, le rayon est parallèle
            //    au sol : pas d'intersection.
            if (std::abs(ray.y) < 1e-6f) {
                return std::nullopt;
            }
            const float t {-origin.y / ray.y};
            if (t < 0.0f) {
                return std::nullopt;   // le sol serait derrière la caméra
            }
            return origin + ray * t;   // le point touché, en (x, 0, z)
        }

    private:
        // Calcule le vecteur "avant" (unitaire) à partir des angles yaw/pitch.
        // C'est de la trigonométrie : cos/sin convertissent des angles en direction.
        [[nodiscard]] glm::vec3 direction() const {
            return {std::cos(pitch_) * std::sin(yaw_),
                    std::sin(pitch_),
                    std::cos(pitch_) * std::cos(yaw_)};
        }

        glm::vec3 position_ {0.0f, 12.0f, -30.0f};  // point de vue initial (x, y, z)
        glm::vec3 up_       {0.0f, 1.0f, 0.0f};      // "le haut" du monde = axe Y

        float yaw_   {0.0f};                  // angle horizontal (radians), 0 = regarde vers +Z
        float pitch_ {glm::radians(-15.0f)};  // angle vertical (radians), négatif = regarde vers le bas

        float speed_ {25.0f};                 // vitesse de vol (mètres par seconde)

        // Limites de pitch pour éviter de se retourner à la verticale.
        float min_pitch_ {glm::radians(-89.0f)};
        float max_pitch_ {glm::radians(89.0f)};

        float fov_  {glm::radians(60.0f)};    // champ de vision (60°)
        float near_ {1.0f};                   // plan proche : rien de plus près n'est dessiné
        float far_  {5000.0f};                // plan lointain : rien de plus loin n'est dessiné
    };
}

#endif //CINDER_CITY_CAMERA_HPP
