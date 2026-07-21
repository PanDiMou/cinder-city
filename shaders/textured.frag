#version 450
// ----------------------------------------------------------------------------
// textured.frag — shader de FRAGMENT pour les modèles texturés.
// Deux textures :
//   - "palette"  : l'atlas de couleur (murs, toit... fenêtres NEUTRES).
//   - "emissive" : l'émissif (lumière des fenêtres/enseignes). Noir ailleurs.
// Le pixel final = palette + émissif : l'émissif AJOUTE la lueur des fenêtres.
// ----------------------------------------------------------------------------

// Deux samplers de fragment (set = 2 : convention shadercross), bindings 0 et 1.
layout(set = 2, binding = 0) uniform sampler2D palette;
layout(set = 2, binding = 1) uniform sampler2D emissive;

layout(location = 0) in vec2 in_uv;        // UV interpolée, reçue du vertex shader
layout(location = 0) out vec4 out_color;   // couleur finale du pixel

// Interrupteur émissif : 0.0 = désactivé (aucune lueur), 1.0 = activé.
// L'émissif reste branché (2 samplers), on coupe juste sa contribution.
// Pour réactiver : passe à 1.0 et recompile le shader.
const float emissive_on = 0.0;

void main() {
    vec4 base = texture(palette, in_uv);
    vec3 glow = texture(emissive, in_uv).rgb;   // lueur (noir là où il n'y a pas de lumière)
    // Couleur de jour + lueur des fenêtres/enseignes (multipliée par l'interrupteur).
    out_color = vec4(base.rgb + glow * emissive_on, base.a);
}
