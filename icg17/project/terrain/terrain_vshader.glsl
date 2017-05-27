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
        //vec2 uv = mod((0.996f * uv_base + 0.002f) + vec2(0.998f) * (uv_base), 1.0f);
        vec2 uv = texture_coordinates * 0.996 + 0.002;
        //vec2 new_uv = uv;
        vec2 new_uv = (uv / float(height_mat_size)) + ((1.0f/float(height_mat_size)) * vec2(col, row));
    
        //float height = 0;
        float height = sandMin + 0.001 * texture(waveheight, new_uv).z;
        vec2 new_xy = (texture(waveheight, new_uv).xy * 2) - 1;
        //position3D = vec3(new_xy.x, height, new_xy.y);
        position3D = vec3(position.x, height, position.y);
        //pos_3d = vec3(vpoint.x, vpoint.y, 0.0f);

        wavenormal_vec = vec3(-texture(wavenormal, new_uv).r, -texture(wavenormal, new_uv).g, texture(wavenormal, new_uv).b);

        // vec2 uv = (position + vec2(1.0, 1.0)) * 0.5;
        // float new_x = uv[0];
        // float new_y = uv[1];
        // if (height < sandMin + 0.005) {
        //     const int num_waves = 5;
        //     // defining the properties of all waves on screen
        //     // amplitude
        //     float a[num_waves] = float[](0.001f,
        //                                  0.002f,
        //                                  0.001f,
        //                                  0.002f,
        //                                  0.001f);
        //     // wavenumber
        //     float w[num_waves] = float[](400.0f,
        //                                  200.0f,
        //                                  200.0f,
        //                                  200.0f,
        //                                  200.0f);
        //     // wave roundness
        //     float q[num_waves] = float[](0.005f,
        //                                  0.005f,
        //                                  0.005f,
        //                                  0.005f,
        //                                  0.005f);
        //     // wave speed
        //     float o[num_waves] = float[](1.0f,
        //                                  2.0f,
        //                                  3.5f,
        //                                  2.0f,
        //                                  2.0f);
        //     // direction
        //     vec2 d[num_waves] = vec2[](vec2(0.0f,2.0f) - uv,
        //                                vec2(1.0f,0.0f) - uv,
        //                                vec2(1.6f,1.6f) - uv,
        //                                vec2(0.0f,1.0f),
        //                                vec2(0.1f,0.9f));

        //     // sending height to fshader so I can debug via color
        //     height = sandMin;
        //     // the new x and y
        //     float temp = 0;
        //     float x_norm = 0;
        //     float y_norm = 0;

        //     // iterating over all the waves and calculating the x, y and height for
        //     // a single point at uv
        //     for (int i = 0;  i < 5; i++) {
        //         temp = w[i] * dot(uv, d[i]) + (time * o[i]);
        //         height += a[i] * 2 * pow((sin(temp) + a[i])/2, 1.5f);
        //         new_x += a[i] * q[i] * d[i][0] * cos(temp);
        //         new_y += a[i] * q[i] * d[i][1] * cos(temp);
        //     }
        //     wavenormal = vec3(-x_norm, -y_norm, 1);
        // } else if (height > sandMin + 0.005 && height > sandMin + 0.01) {
        //     wavenormal = vec3(0,0,1);
        //     position3D = vec3((2 * new_x - 1), height + 0.0001, (2 * new_y - 1));
        // }
        // position3D = vec3((2 * new_x - 1), height, (2 * new_y - 1));
    } else {
        wavenormal_vec = vec3(0,0,1);
        // 3D vertex position : X and Y from vertex array, Z from heightmap texture.
        position3D = vec3(position.x, height, position.y);
    }

    if(isReflection) {
        if (height < sandMin) {
            position3D = vec3(position.x, sandMin + 0.0001, position.y);
        } else {
            position3D = vec3(position3D.x, (sandMin*2)-position3D.y, position3D.z);
        }
    }

    mv = view * model;
    vpoint_mv = mv * vec4(position3D, 1.0);

    gl_Position = projection * vpoint_mv;

    light_dir = normalize(light_pos - vpoint_mv.xyz);
    view_dir = normalize(position3D - vpoint_mv.xyz);
}
