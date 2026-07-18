#version 450

layout(location = 0) in vec3 in_position;

layout(set = 1, binding = 0) uniform Transform {
    mat4 model_view_projection;
};

layout(location = 0) out vec3 v_world;

void main() {
    gl_Position = model_view_projection * vec4(in_position, 1.0);
    v_world = in_position; // ground is at identity transform: local == world position
}
