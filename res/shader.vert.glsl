#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coords;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

layout (location = 0) out vec3 v_position;
layout (location = 1) out vec3 v_world_position;
layout (location = 2) out vec3 v_normal;

void main() {
   v_position = position;
   v_world_position = (model * vec4(position, 1.0)).xyz;
   
   mat3 normalMatrix = transpose(inverse(mat3(model)));
   
   v_normal = normalize(normalMatrix * position);

   gl_Position = perspective * view * model * vec4(position, 1.0);
}