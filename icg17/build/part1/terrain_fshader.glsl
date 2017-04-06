#version 330

in vec2 uv;

uniform sampler2D heightMap;

out vec3 color;

void main() {
	float height = texture(heightMap, uv).r;
	if(height == 0) {
    	color = vec3(0.0f, 0.0f, 1.0f);
    }else{
       color = vec3(0.0f, 1.0f, 0.0f);
    }
}
