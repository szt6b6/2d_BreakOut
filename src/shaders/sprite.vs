#version 330
layout (location=0) in vec4 vertex; // vertex point and texture coords

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection; // 2d world, view matrix is no need

void main() {
    TexCoords = vertex.zw;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}