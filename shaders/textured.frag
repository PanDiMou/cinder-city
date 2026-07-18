#version 450

// Combined texture + sampler (fragment stage): shadercross convention -> set = 2.
layout(set = 2, binding = 0) uniform sampler2D palette;

layout(location = 0) in vec2 in_uv;
layout(location = 0) out vec4 out_color;

void main() {
    // Look up the flat palette color pointed to by this fragment's UV.
    out_color = texture(palette, in_uv);
}
