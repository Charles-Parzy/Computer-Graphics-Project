#version 330

in vec2 position;

uniform sampler2D heightMap;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
uniform bool isWater;

out vec4 vpoint_mv;
out vec3 light_dir, view_dir;
out vec2 texture_coordinates;

const float sandMin = 0.131f; // Keep it consistant with a little bit more 

void main() {
    // World (triangle grid) coordinates are (-1,-1) x (1,1).
    // Texture (height map) coordinates are (0,0) x (1,1).
    texture_coordinates = (position + vec2(1.0, 1.0)) * 0.5;
    float height = texture(heightMap, texture_coordinates).r;

    vec3 vertexPosition3DWorld;

    if(isWater) {
        vertexPosition3DWorld = vec3(position.x, sandMin, position.y);
    } else {
        // 3D vertex position : X and Y from vertex array, Z from heightmap texture.
        vertexPosition3DWorld = vec3(position.x, height, position.y);
    }

    mat4 MV = view * model;
    vpoint_mv = MV * vec4(vertexPosition3DWorld, 1.0);

   	gl_Position = projection * vpoint_mv;

    light_dir = normalize(light_pos - vpoint_mv.xyz);
    view_dir = normalize(vertexPosition3DWorld - vpoint_mv.xyz);
}
