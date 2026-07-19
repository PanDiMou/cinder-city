#version 450
// ----------------------------------------------------------------------------
// grid_floor.vert — shader de SOMMET pour le sol quadrillé.
// En plus de placer le sommet, il transmet sa position au fragment shader, qui
// s'en sert pour dessiner les lignes du quadrillage.
// ----------------------------------------------------------------------------

layout(location = 0) in vec3 in_position;

layout(set = 1, binding = 0) uniform Transform {
    mat4 model_view_projection;   // matrice MVP (place le sommet à l'écran)
};

// "out" ici = donnée transmise au fragment shader (le GPU l'interpole entre les
// sommets). On lui envoie la position dans le monde.
layout(location = 0) out vec3 v_world;

void main() {
    gl_Position = model_view_projection * vec4(in_position, 1.0);
    // Le sol n'a aucune transformation (il est à l'origine, sans rotation ni échelle),
    // donc sa position locale EST déjà sa position dans le monde. Pratique.
    v_world = in_position;
}
