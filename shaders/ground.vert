#version 450

// Per-vertex input: matches the pipeline's vertex layout (location 0 = position).
layout(location = 0) in vec3 in_position;

// Uniform buffer (vertex stage): SDL_gpu/shadercross convention -> set = 1.
layout(set = 1, binding = 0) uniform Camera {
    mat4 view_projection;
};

void main() {
    // Place the vertex on screen: world position * camera matrix.
    gl_Position = view_projection * vec4(in_position, 1.0);
}
