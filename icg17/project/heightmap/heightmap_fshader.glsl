#version 330

in vec2 uv;

out vec3 height;

uniform float Gain;
uniform float Lacunarity;
uniform float H;
uniform float Offset;
uniform int Octaves;

const float heightScaleFactor = 0.2;
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

const int permutations[] = int[](151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7,
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


// Perlin noise interpolation function. ( f(t) = 6t^5 - 15t^4 + 10t^3 )
vec2 applyInterpolationFunction(vec2 t) {
    return t * t * t * ( t * (t*6-15) + 10 );
}

// Look-up in the permutations table, and select a value.
int getPermutation(float idx) {
    int index = int(idx); // cast to int
    int correctIndex = int(mod(index, 255));
    return permutations[correctIndex];
}

// Look-up in the getGradientient table, and select a getGradientient vector
// Then we compute the cross product between the getGradientient and the position vector.
float getGradient(int idx, vec2 position) {
    int correctIndex = int(mod(idx, 8));
    return dot(gradients[correctIndex], position);
}

float mixFunction(float a, float b, float t) {
    return a + t*(b-a);
}

float perlin_noise(vec2 position) {

    // Cell containing the pixel (bottom left corner)
    vec2 cell = vec2(floor(position));

    // Find position of the pixel in the cell. Vector from bottom left corner to pixel = a.
    vec2 pixelPos = position - vec2(cell);

    // Generate a pseudo random gradient for each corner

    // Bottom left corner
    int pseudoRandom = getPermutation( getPermutation(cell.x) + cell.y );
    float vecBL = getGradient(pseudoRandom, pixelPos.xy); // g(x,y) * a

    // Bottom right corner
    pseudoRandom = getPermutation( getPermutation(cell.x+1) + cell.y+0 );
    float vecBR  = getGradient(pseudoRandom, pixelPos.xy -vec2(1,0)); // g(x,y) * b

    // Top left corner
    pseudoRandom = getPermutation( getPermutation(cell.x+0) + cell.y+1 );
    float vecTL  = getGradient(pseudoRandom, pixelPos.xy -vec2(0,1)); // g(x,y) * c

    // Top right corner
    pseudoRandom = getPermutation( getPermutation(cell.x+1) + cell.y+1 );
    float vecTR = getGradient(pseudoRandom, pixelPos.xy -vec2(1,1)); // g(x,y) * d

    vec2 f = applyInterpolationFunction(pixelPos.xy);

    float s_t_ = mixFunction(vecBL, vecBR, f.x);
    float u_v_ = mixFunction(vecTL, vecTR, f.x);
    return mixFunction(s_t_, u_v_, f.y);
}


// Apply the function = f(x) = Sum of l^(iH) * f(l^i * x) where i is from 0 to octaves
float fBm(vec2 point, float H, float lacunarity, int octaves) {
    float value = 0.0f;

    for (int i=0; i < octaves; i++) {
        value += (perlin_noise(point) * pow(lacunarity, -H*i));
        point *= lacunarity;
    }
    return value;
}


float hybridMultifractal(vec2 point, float H, float lacunarity, int octaves, float offset) {
    //first component
    float frequency = 0.6f;
    float weight = (perlin_noise(1.5f*point) + offset) * pow(frequency, -H);
    float sgnl = 0.0f;
    float height =  weight;
    point *= lacunarity;

    for (int k=1; k<octaves; k++) {
        if ( weight > 1.0f )
            weight = 1.0f;
        frequency *= lacunarity;
        sgnl = (perlin_noise(1.75f*point)+ offset ) * pow(frequency, -H);;
        height += weight * sgnl;
        weight *= sgnl;
        point *= lacunarity;
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
   height = vec3(ridgedMultifractal(uv, H, Lacunarity, Octaves, Offset, Gain) * heightScaleFactor);
}
