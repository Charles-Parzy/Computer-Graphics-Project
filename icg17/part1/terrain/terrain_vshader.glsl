#version 330

in vec2 position;

uniform sampler2D heightMap;
uniform mat4 MVP;

out vec2 uv;

void main() {
    // World (triangle grid) coordinates are (-1,-1) x (1,1).
    // Texture (height map) coordinates are (0,0) x (1,1).
    uv = (position + vec2(1.0, 1.0)) * 0.5;
    float height = texture(heightMap, uv).r;

    // 3D vertex position : X and Y from vertex array, Z from heightmap texture.
    vec3 vertexPosition3DWorld = vec3(position.x, height, position.y);

   	gl_Position = MVP * vec4(vertexPosition3DWorld, 1.0);
}
