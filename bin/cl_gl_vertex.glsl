#version 330 core

layout(location = 0) in vec2 position;

out vec2 uv;

void main() {
    gl_Position = position;
    uv = 0.5 * position + vec2(0.5, 0.5);
}