#version 450

layout(set = 3, binding = 0) uniform Material {
    vec4 color; // base color (gray)
};

layout(location = 0) in vec3 v_world;
layout(location = 0) out vec4 out_color;

// Anti-aliased grid line intensity for a given cell size (in meters).
float grid(vec2 world_xz, float cell) {
    vec2 coord = world_xz / cell;
    vec2 g = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);
    return 1.0 - min(min(g.x, g.y), 1.0);
}

void main() {
    vec2 xz = v_world.xz;
    float fine  = grid(xz, 1.0);    // lines every 1 m
    float major = grid(xz, 10.0);   // brighter lines every 10 m

    vec3 c = mix(color.rgb, vec3(0.45), fine * 0.5);
    c = mix(c, vec3(0.70), major);
    out_color = vec4(c, 1.0);
}
