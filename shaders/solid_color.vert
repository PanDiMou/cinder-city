#version 450
// ----------------------------------------------------------------------------
// solid_color.vert — shader de SOMMET pour le matériau "couleur unie".
// Un shader de sommet s'exécute une fois par sommet ; son but : calculer la
// position finale du sommet à l'écran (gl_Position).
// ----------------------------------------------------------------------------

// "in" = donnée reçue pour ce sommet, lue depuis le buffer de sommets.
// location 0 doit correspondre à l'attribut "position" déclaré dans renderer.cpp.
layout(location = 0) in vec3 in_position;

// "uniform" = donnée identique pour tous les sommets de l'objet (ici la matrice
// caméra). set = 1 est la convention SDL_gpu/shadercross pour un uniform de sommet.
layout(set = 1, binding = 0) uniform Camera {
    mat4 view_projection;   // en réalité la matrice MVP complète (caméra * objet)
};

void main() {
    // vec4(in_position, 1.0) : on passe de 3 à 4 composantes (le 1.0 permet à la
    // matrice d'appliquer aussi les déplacements). gl_Position = position à l'écran.
    gl_Position = view_projection * vec4(in_position, 1.0);
}
