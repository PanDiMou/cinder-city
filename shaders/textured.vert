#version 450
// ----------------------------------------------------------------------------
// textured.vert — shader de SOMMET pour les modèles texturés (bâtiments).
// Il place le sommet à l'écran ET transmet sa coordonnée de texture (UV) au
// fragment shader, qui s'en servira pour lire la bonne couleur dans la palette.
// ----------------------------------------------------------------------------

// Deux entrées par sommet cette fois : la position ET la coordonnée de texture.
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

layout(set = 1, binding = 0) uniform Camera {
    mat4 view_projection;   // matrice MVP
};

// On transmet l'UV au fragment shader (le GPU l'interpole sur chaque triangle).
layout(location = 0) out vec2 out_uv;

void main() {
    gl_Position = view_projection * vec4(in_position, 1.0);
    out_uv = in_uv;   // on fait juste "passer" l'UV, sans calcul
}
