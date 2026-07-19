#version 450
// ----------------------------------------------------------------------------
// grid_floor.frag — shader de FRAGMENT pour le sol quadrillé (style Blender).
// Il calcule, pour chaque pixel du sol, s'il tombe sur une ligne du quadrillage,
// et mélange les couleurs en conséquence. Tout est PROCÉDURAL (calculé), sans texture.
// ----------------------------------------------------------------------------

layout(set = 3, binding = 0) uniform Material {
    vec4 color; // couleur de base (gris)
};

layout(location = 0) in vec3 v_world;      // position monde reçue du vertex shader
layout(location = 0) out vec4 out_color;   // couleur finale du pixel

// Renvoie l'intensité d'une ligne (0 = pas de ligne, 1 = pleine ligne) pour une
// grille dont les cases font `cell` mètres. La division par fwidth (la vitesse de
// changement de la coordonnée à l'écran) sert à l'anti-aliasing : les lignes
// restent nettes de près comme de loin, sans scintiller.
float grid(vec2 world_xz, float cell) {
    vec2 coord = world_xz / cell;                       // combien de cases depuis l'origine
    vec2 g = abs(fract(coord - 0.5) - 0.5) / fwidth(coord); // distance au bord de case
    return 1.0 - min(min(g.x, g.y), 1.0);
}

void main() {
    vec2 xz = v_world.xz;            // on travaille dans le plan horizontal (X, Z)
    float fine  = grid(xz, 1.0);    // fines lignes tous les 1 m
    float major = grid(xz, 10.0);   // lignes plus marquées tous les 10 m

    // mix(a, b, t) = interpolation : t=0 donne a, t=1 donne b. On part de la couleur
    // de base, on éclaircit un peu sur les fines lignes, puis davantage sur les grandes.
    vec3 c = mix(color.rgb, vec3(0.45), fine * 0.5);
    c = mix(c, vec3(0.70), major);
    out_color = vec4(c, 1.0);       // alpha = 1 (opaque)
}
