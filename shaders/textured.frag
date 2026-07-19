#version 450
// ----------------------------------------------------------------------------
// textured.frag — shader de FRAGMENT pour les modèles texturés.
// Pour chaque pixel, il lit sa couleur dans la texture "palette" à l'endroit
// indiqué par l'UV. C'est le principe du look POLYGON de Synty : chaque face
// pointe vers une petite case de couleur plate dans l'atlas.
// ----------------------------------------------------------------------------

// sampler2D = la texture + ses règles de lecture, réunies. set = 2 est la
// convention shadercross pour un sampler de fragment.
layout(set = 2, binding = 0) uniform sampler2D palette;

layout(location = 0) in vec2 in_uv;        // UV interpolée, reçue du vertex shader
layout(location = 0) out vec4 out_color;   // couleur finale du pixel

void main() {
    // texture(sampler, uv) va lire la couleur dans la palette à la coordonnée uv.
    out_color = texture(palette, in_uv);
}
