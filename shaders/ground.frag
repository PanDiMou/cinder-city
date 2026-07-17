#version 450

// Fragment uniforms: set 3 (SDL_gpu / shadercross convention).
layout(set = 3, binding = 0) uniform Material {
    vec4 color;
};

layout(location = 0) out vec4 out_color;

void main() {
    out_color = color;
}
