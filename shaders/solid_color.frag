#version 450
// ----------------------------------------------------------------------------
// solid_color.frag — shader de FRAGMENT pour le matériau "couleur unie".
// Un shader de fragment s'exécute une fois par pixel ; son but : donner la
// couleur finale de ce pixel (out_color). Ici, la même couleur partout.
// ----------------------------------------------------------------------------

// Uniform de fragment (set = 3 : convention SDL_gpu/shadercross). La couleur est
// envoyée depuis renderer.cpp (SDL_PushGPUFragmentUniformData).
layout(set = 3, binding = 0) uniform Material {
    vec4 color;   // rouge, vert, bleu, alpha
};

// "out" = la sortie du shader : la couleur du pixel, envoyée à l'écran.
layout(location = 0) out vec4 out_color;

void main() {
    out_color = color;   // on recopie simplement la couleur du matériau
}
