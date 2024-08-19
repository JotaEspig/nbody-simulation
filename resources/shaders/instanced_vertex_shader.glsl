#version 460 core

layout(location = 0) in vec3 axolote_aPos;
layout(location = 1) in vec4 axolote_aColor;
layout(location = 4) in mat4 aInstanceMat;

out vec4 color;

uniform mat4 axolote_projection;
uniform mat4 axolote_view;

mat4 camera() {
    return axolote_projection * axolote_view;
}

void main() {
    // vec3 current_pos = vec3(mat * vec4(axolote_aPos, 1.0f));
    // gl_Position = camera() * vec4(current_pos, 1.0f);
    color = axolote_aColor;
    gl_Position = camera() * aInstanceMat * vec4(axolote_aPos, 1.0f);
}
