#version 330

in vec2 position;

out vec2 uv;
out float height;

uniform mat4 MVP;
uniform float time;

const int num_waves = 5;
const bool water = false;

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    // convert the 2D position into 3D positions that all lay in a horizontal
    // plane.
    // TODO 6: animate the height of the grid points as a sine function of the
    // 'time' and the position ('uv') within the grid.
    
    vec3 pos_3d;
    
    if (water) {
        // defining the properties of all waves on screen
        // amplitude
        float a[num_waves] = float[](0.01f * sin(time) + 0.01f,
                                     0.02f * sin(time + 3.14f) + 0.02f,
                                     0.02f * sin(time + 1.57f) + 0.02f,
                                     0.03f,
                                     0.03f);
        // wavenumber
        float w[num_waves] = float[](20.0f,
                                     20.0f,
                                     20.0f,
                                     20.0f,
                                     20.0f);
        // wave roundness
        float q[num_waves] = float[](0.05f,
                                     0.05f,
                                     0.05f,
                                     0.05f,
                                     0.05);
        // wave speed
        float o[num_waves] = float[](10.0f,
                                     5.0f,
                                     7.5f,
                                     5.0f,
                                     5.0f);
        // direction
        vec2 d[num_waves] = vec2[](vec2(0.0f,2.0f) - uv,
                                   vec2(1.0f,0.0f) - uv,
                                   vec2(1.6f,1.6f) - uv,
                                   vec2(0.0f,1.0f),
                                   vec2(0.1f,0.9f));
    
        // sending height to fshader so I can debug via color
        height = 0;
        // the new x and y
        float new_x = uv[0];
        float new_y = uv[1];
        float temp = 0;
    
        // iterating over all the waves and calculating the x, y and height for
        // a single point at uv
        for (int i = 0;  i < num_waves; i++) {
            temp = w[i] * dot(uv, d[i]) + (time * o[i]);
            height += a[i] * 2 * pow((sin(temp) + a[i])/2, 1.5f) - a[i];
            new_x += a[i] * q[i] * d[i][0] * cos(temp);
            new_y += a[i] * q[i] * d[i][1] * cos(temp);
        }
        pos_3d = vec3((2 * new_x - 1), height, (2 * new_y - 1));
    } else {
        float temp = 20.0f * dot(uv, vec2(1.0,1.0)) + (time * 5);
        height = 0.03f * sin(temp) - 0.03f;
        pos_3d = vec3(position[0], height, position[1]);
    }
    gl_Position = MVP * vec4(pos_3d, 1.0);
}
