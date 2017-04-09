#version 330

in vec2 uv;

out vec3 height;

const vec2 gradients[] = vec2[](
            vec2(1.0f,  1.0f),
            vec2(-1.0f,  1.0f),
            vec2(1.0f, -1.0f),
            vec2(-1.0f, -1.0f),
            vec2(0.0f,  1.0f),
            vec2(0.0f, -1.0f),
            vec2(1.0f,  0.0f),
            vec2(-1.0f,  0.0f)
    );

const int permutation[] = int[](151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7,
                                225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
                                120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
                                88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134,
                                139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220,
                                105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80,
                                73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
                                164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38,
                                147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189,
                                28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101,
                                155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232,
                                178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12,
                                191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181,
                                199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236,
                                205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180);


// Perlin's interpolation function.
vec2 fade_new(vec2 t) {
    return t * t * t * ( t * (t*6-15) + 10 );
}

// Look-up in the permutation table.
int perm(int idx) {
    int idx_mod = int(mod(idx, 255));
    return permutation[idx_mod];
}

// Look-up in the gradient table.
float grad(int idx, vec2 position) {
    int idx_mod = int(mod(idx, 8));   
    return dot(gradients[idx_mod], position);
}

// Linear interpolation.
float lerp(in float a, in float b, in float t) {
    return a + t*(b-a);
}

// Perlin noise function.
float perlin_noise(vec2 position) {

    // Find which square contains the pixel.
    ivec2 square = ivec2(floor(position));

    // Find relative position (displacement) in this square [0,1].
    vec2 disp = position - vec2(square);

    // Generate a pseudo-random value for current square using an hash function.
    int rnd = perm( perm(square.x) + square.y );

    // Noise at current position.
    float noise = grad(rnd, disp.xy);

    // Noise at the 3 neightboring positions.
    rnd = perm( perm(square.x+1) + square.y+0 );
    float noise_x  = grad(rnd, disp.xy -vec2(1,0));
    rnd = perm( perm(square.x+0) + square.y+1 );
    float noise_y  = grad(rnd, disp.xy -vec2(0,1));
    rnd = perm( perm(square.x+1) + square.y+1 );
    float noise_xy = grad(rnd, disp.xy -vec2(1,1));

    // Fade curve.
    vec2 f = fade_new(disp.xy);

    // Average over the four neighbor pixels.
    float avg1 = lerp(noise,   noise_x,  f.x);
    float avg2 = lerp(noise_y, noise_xy, f.x);
    return lerp(avg1, avg2, f.y);
}


// Fractal Brownian motion : sum_k l^(-k*H) * f(l^k * x).
float fBm(vec2 position, float H, float lacunarity, int octaves) {
    float height = 0.0f;

    for (int k=0; k<octaves; k++) {
        height   += (perlin_noise(position) * pow(lacunarity, -H*k));
        position *= lacunarity;
    }
    return height;
}


float hybridMultifractal(vec2 position, float H, float lacunarity, int octaves, float offset) {
    //first component
    float frequency = 0.6f;
    float weight = (perlin_noise(1.5f*position)+ offset )  * pow(frequency, -H);
    float signal = 0.0f;
    float height =  weight;
    position *= lacunarity;

    for (int k=1; k<octaves; k++) {
        if ( weight > 1.0f )
            weight = 1.0f;
        frequency *= lacunarity;
        signal = (perlin_noise(1.75f*position)+ offset )  * pow(frequency, -H);;
        height += weight * signal;
        weight *= signal;
        position *= lacunarity;
    }

    return ((height - 1.5f)/6.0f + 0.035f);

}

float ridgedMultifractal(vec2 p, float H, float lacunarity, int octaves, float offset, float gain) {
    float result, frequency, signal, weight;

    frequency = 0.9f;

    signal = offset - abs(perlin_noise(p));
    signal*= signal;
    result = signal;
    weight = 1.0;

    for(int i=1; i<octaves; ++i) {
        p*= lacunarity;
        weight = clamp(signal*gain, 0.0,1.0);
        signal = offset - abs(perlin_noise(p));
        signal*= signal*weight;
        result+= signal * pow(frequency, -H);
        frequency*= lacunarity;
    }

    return result;

}

void main() {
   //height = vec3(perlin_noise(uv*10));
   //height = vec3(fBm(uv, 0.25f, 2.3f, 4));
   //height = vec3(hybridMultifractal(uv, 0.25f, 2.3f, 15, 0.7f));
   height = vec3(min(0.45, ridgedMultifractal(uv, 0.25f, 2.3f, 15, 0.695f, 2.1f)*0.2)); //small HACK since there was a really high value
}


