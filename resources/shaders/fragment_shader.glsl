#version 330 core

out vec4 FragColor;

in vec3 color;

uniform vec3 color_uniform;
uniform bool is_color_uniform_set;

void main() {
    if (is_color_uniform_set) {
        FragColor = vec4(color_uniform, 1.0f);
    }
    else {
        FragColor = vec4(color, 1.0f);
    }
}
