#version 330 core

uniform sampler2D sampler1;
uniform sampler2D sampler2;

in vec2 fragment_uv;

out vec3 color;

void main() {
    vec3 color1 = texture(sampler1, fragment_uv).rgb;
    vec3 color2 = texture(sampler2, fragment_uv).rgb;
    color = abs(color1 - color2) * 2;
}
