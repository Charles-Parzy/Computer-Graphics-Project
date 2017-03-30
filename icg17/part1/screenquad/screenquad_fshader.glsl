#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;
uniform int horizontal;
uniform float variance;

vec3 gaussianBlur() {
    vec3 result = vec3(0.0f);
    float total_weight = 0.0f;
    int SIZE = 1 + 2*3*int(ceil(sqrt(variance)));
    for(int i=-SIZE; i <= SIZE; i++) {
        float w = exp(-(i*i)/(2.0*variance*variance));

        float x = horizontal==0 ? i/tex_width : 0;
        float y = horizontal==0 ? 0 : i/tex_height;

        vec3 neighbor = texture(tex, uv+vec2(x,y)).rgb;
        result += w * neighbor;
        total_weight += w;
    }
    return result/total_weight;
}

void main() {
    color = gaussianBlur();
}

