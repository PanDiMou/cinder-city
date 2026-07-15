#version 450

layout(location = 0) out vec4 out_color;

void main() {
    // Flat ground color for now (a grid/texture can come later).
    out_color = vec4(0.23, 0.44, 0.24, 1.0);
}
