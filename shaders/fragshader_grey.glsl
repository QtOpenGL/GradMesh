#version 410
// Fragment shader
layout (location = 1) in vec3 in_color;

out vec4 fColor;

void main() {

  fColor = vec4(0.5, 0.5, 0.5, 1.0);

}
