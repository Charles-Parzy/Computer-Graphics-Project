#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;

void main() {
    color = vec3(0.0f, 0.0f, 0.0f);
}

