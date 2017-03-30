#version 330

in vec2 position;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(position.x, 0.0, position.y, 1.0);
}
