#version 430 core
layout(location = 0) in vec3 axolote_aPos;
layout(location = 1) in vec4 axolote_aColor;
layout(location = 2) in vec2 axolote_aTex;
layout(location = 3) in vec3 axolote_aNormal;
layout(std430, binding = 0) buffer Displacements {
    float displacements[];
};

out vec4 axolote_in_color;
out vec2 axolote_in_tex_coord;
out vec3 axolote_in_current_pos;

uniform mat4 axolote_gmesh_model;
uniform mat4 axolote_scene_camera;

void main() {
    mat4 translation = mat4(1.0);
    translation[3] = vec4(0, -displacements[gl_VertexID], 0, 1);

    mat4 model = axolote_gmesh_model * translation;
    axolote_in_current_pos = vec3(model * vec4(axolote_aPos, 1.0f));
    axolote_in_tex_coord = axolote_aTex;
    axolote_in_color = axolote_aColor;

    gl_Position = axolote_scene_camera * vec4(axolote_in_current_pos, 1.0f);
}
