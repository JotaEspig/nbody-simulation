#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aNormal;

out vec3 color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

mat4 camera() {
    return projection * view;
}

void main() {
    vec3 current_pos = vec3(model * vec4(aPos, 1.0f));
    color = aColor;
    gl_Position = camera() * vec4(current_pos, 1.0f);
}
