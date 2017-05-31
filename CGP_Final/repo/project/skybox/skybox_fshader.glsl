#version 330 core

uniform sampler2D tex;
uniform bool isReflection;

in vec2 uv;

out vec3 color;

void main(){
	if (isReflection) {
		color = texture(tex, vec2(1-uv.x, uv.y)).rgb;
	} else {
		color = texture(tex, uv).rgb;
	}
}
