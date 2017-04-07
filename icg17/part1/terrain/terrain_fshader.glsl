#version 330

in vec2 uv;

uniform sampler2D heightMap;

out vec3 color;

void main() {
/*
const float ground = 0.000;  //water height level
const float sandMin = 0.000;
const float forestMin = 0.015;
const float snowMin = 0.16;
*/
		float height = texture(heightMap, uv).r;
    	if (height >= 0.16f) {
    	    	color = vec3(1.0f, 0.98f, 0.98f);
    	}else if(height >= 0.015 && height < 0.21f) {
    	    	color = vec3(0.14f, 0.55f, 0.14f);
    	}else if(height > 0.0 && height <= 0.015) {
    	    	color = vec3(0.76f, 0.69f, 0.5f);
    	}else {
    	    color = vec3(0.25f, 0.64f, 0.88f);
    	}
}
