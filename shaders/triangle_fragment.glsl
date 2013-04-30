#version 330 core

uniform vec4 triangle_color;

out vec4 color;

void main() {
    color = triangle_color;
}
