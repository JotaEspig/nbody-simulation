#version 460 core

layout(location = 0) in vec3 axolote_aPos;
//layout(location = 1) in vec3 aColor;
//layout(location = 4) in mat4 aInstanceMat;

out vec3 color;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 inColor;

mat4 camera() {
    return projection * view;
}

void main() {
    // vec3 current_pos = vec3(mat * vec4(axolote_aPos, 1.0f));
    // gl_Position = camera() * vec4(current_pos, 1.0f);
    color = inColor;
    // gl_Position = camera() * aInstanceMat * vec4(axolote_aPos, 1.0f);
    gl_Position = camera() * vec4(axolote_aPos, 1.0f);
}
