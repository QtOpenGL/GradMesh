#version 410
// Vertex shader

layout (location = 0) in vec2 vertcoords_clip_vs;
layout (location = 1) in vec3 in_colour;

uniform float dx = 0.0f;
uniform float dy = 0.0f;
uniform float scale = 1.0f;

layout (location = 1) out vec3 out_colour;

void main() {
    vec2 coords = scale * vertcoords_clip_vs;
    coords -= vec2(dx, dy);
    gl_Position = vec4(coords, 0.0, 1.0);
    out_colour = in_colour;
}
