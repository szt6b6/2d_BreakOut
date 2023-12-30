#version 330

in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D image;

void main() {
    color = texture(image, TexCoords) * ParticleColor;
    // color = vec4(1.0, 0.0, 0.0, 1.0);
}
