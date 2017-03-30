#version 330

in vec2 position;

out vec2 uv;

uniform mat4 MVP;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;
    gl_Position = MVP * uv;
}
