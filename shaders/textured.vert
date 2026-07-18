#version 450

// Per-vertex input: matches the pipeline's vertex layout.
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

// Uniform buffer (vertex stage): SDL_gpu/shadercross convention -> set = 1.
layout(set = 1, binding = 0) uniform Camera {
    mat4 view_projection;
};

// Pass the texture coordinates through to the fragment stage.
layout(location = 0) out vec2 out_uv;

void main() {
    gl_Position = view_projection * vec4(in_position, 1.0);
    out_uv = in_uv;
}
