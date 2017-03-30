#version 330

in vec2 uv;
in float height;

out vec3 color;

uniform sampler2D tex;

const bool debug = false;

void main() {
    if (debug) {
        color = vec3(0, 0, 1 - abs(height/0.16f));
    } else {
        color = texture(tex, uv).rgb;
    }
}
