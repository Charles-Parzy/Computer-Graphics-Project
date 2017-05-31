#version 330

in vec2 position;

uniform sampler2D heightMap;
uniform sampler2D waveheight;
uniform sampler2D wavenormal;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
uniform bool isWater;
uniform bool isReflection;
uniform float time;
uniform int row;
uniform int col;
uniform int height_mat_size;

out vec4 vpoint_mv;
out vec3 light_dir, view_dir;
out vec2 texture_coordinates;
out vec3 wavenormal_vec;
out mat4 mv;

const float sandMin = 0.1322f; // Keep it consistant with a little bit more

void main() {
    // World coordinates are from -1 to 1, we map them to texture coordinates
    // which are from 0 to 1.
    texture_coordinates = (position + vec2(1.0, 1.0)) * 0.5;
    float height = texture(heightMap, texture_coordinates).r;
    vec3 position3D;

    if(isWater) {
        vec2 uv = texture_coordinates * 0.996 + 0.002;

        vec2 new_uv = (uv / float(height_mat_size)) + ((1.0f/float(height_mat_size)) * vec2(col, row));
    

        float height = sandMin + 0.001 * texture(waveheight, new_uv).z;
        vec2 new_xy = (texture(waveheight, new_uv).xy * 2) - 1;
        position3D = vec3(new_xy.x, height, new_xy.y);

        wavenormal_vec = vec3(-texture(wavenormal, new_uv).r, -texture(wavenormal, new_uv).g, texture(wavenormal, new_uv).b);

    } else if (isReflection) {
        wavenormal_vec = vec3(0,0,1); 
        if (height < sandMin) {
            position3D = vec3(position.x, sandMin + 0.0001, position.y);
        } else {
            position3D = vec3(position.x, (sandMin*2)-height,  position.y);
        }
    } else {
        wavenormal_vec = vec3(0,0,1);
        // 3D vertex position : X and Y from vertex array, Z from heightmap texture.
        position3D = vec3(position.x, height, position.y);
    }

    mv = view * model;
    vpoint_mv = mv * vec4(position3D, 1.0);

    gl_Position = projection * vpoint_mv;

    light_dir = normalize(light_pos - vpoint_mv.xyz);
    view_dir = normalize(position3D - vpoint_mv.xyz);
}
