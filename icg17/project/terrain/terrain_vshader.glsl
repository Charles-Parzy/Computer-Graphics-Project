#version 330

in vec2 position;

uniform sampler2D heightMap;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
uniform bool isWater;
uniform bool isReflection;
uniform float time;

out vec4 vpoint_mv;
out vec3 light_dir, view_dir;
out vec2 texture_coordinates;
out vec3 wavenormal;
out mat4 mv;

const float sandMin = 0.1322f; // Keep it consistant with a little bit more 

void main() {
    // World (triangle grid) coordinates are (-1,-1) x (1,1).
    // Texture (height map) coordinates are (0,0) x (1,1).
    texture_coordinates = (position + vec2(1.0, 1.0)) * 0.5;
    float height = texture(heightMap, texture_coordinates).r;

    vec3 vertexPosition3DWorld;

    if(isWater) {
        vec2 uv = (position + vec2(1.0, 1.0)) * 0.5;
        float new_x = uv[0];
        float new_y = uv[1];
        if (height < sandMin + 0.005) {
            const int num_waves = 5;
            // defining the properties of all waves on screen
            // amplitude
            float a[num_waves] = float[](0.001f,
                                         0.002f,
                                         0.001f,
                                         0.002f,
                                         0.001f);
            // wavenumber
            float w[num_waves] = float[](400.0f,
                                         200.0f,
                                         200.0f,
                                         200.0f,
                                         200.0f);
            // wave roundness
            float q[num_waves] = float[](0.005f,
                                         0.005f,
                                         0.005f,
                                         0.005f,
                                         0.005f);
            // wave speed
            float o[num_waves] = float[](1.0f,
                                         2.0f,
                                         3.5f,
                                         2.0f,
                                         2.0f);
            // direction
            vec2 d[num_waves] = vec2[](vec2(0.0f,2.0f) - uv,
                                       vec2(1.0f,0.0f) - uv,
                                       vec2(1.6f,1.6f) - uv,
                                       vec2(0.0f,1.0f),
                                       vec2(0.1f,0.9f));
            
            // sending height to fshader so I can debug via color
            height = sandMin;
            // the new x and y
            float temp = 0;
            float x_norm = 0;
            float y_norm = 0;
            
            // iterating over all the waves and calculating the x, y and height for
            // a single point at uv
            for (int i = 0;  i < 5; i++) {
                temp = w[i] * dot(uv, d[i]) + (time * o[i]);
                height += a[i] * 2 * pow((sin(temp) + a[i])/2, 1.5f);
                new_x += a[i] * q[i] * d[i][0] * cos(temp);
                new_y += a[i] * q[i] * d[i][1] * cos(temp);
                //x_norm += w[i] * d[i][0] * a[i] * cos(temp);
                //y_norm += w[i] * d[i][1] * a[i] * cos(temp);
            }
            wavenormal = vec3(-x_norm, -y_norm, 1);
        } else if (height > sandMin + 0.005 && height > sandMin + 0.01) {
            wavenormal = vec3(0,0,1);
            vertexPosition3DWorld = vec3((2 * new_x - 1), height + 0.0001, (2 * new_y - 1));
        }
        vertexPosition3DWorld = vec3((2 * new_x - 1), height, (2 * new_y - 1));
    } else {
        wavenormal = vec3(0,0,1);
        // 3D vertex position : X and Y from vertex array, Z from heightmap texture.
        vertexPosition3DWorld = vec3(position.x, height, position.y);
    }

    if(isReflection) {
        if (height < sandMin) {
            vertexPosition3DWorld = vec3(position.x, sandMin + 0.0001, position.y);
        } else {
            vertexPosition3DWorld = vec3(vertexPosition3DWorld.x, (sandMin*2)-vertexPosition3DWorld.y, vertexPosition3DWorld.z);
        }
    }

    mv = view * model;
    vpoint_mv = mv * vec4(vertexPosition3DWorld, 1.0);

    gl_Position = projection * vpoint_mv;

    light_dir = normalize(light_pos - vpoint_mv.xyz);
    view_dir = normalize(vertexPosition3DWorld - vpoint_mv.xyz);
}