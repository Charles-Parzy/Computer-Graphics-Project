#version 330

in vec2 uv;

out vec3 height;

uniform sampler1D permTex1D;

uniform float tex_width;
uniform float tex_height;

// Perlin's new interpolation function (Hermite polynomial of degree 5).
// Results in a C2 continuous noise function.
vec2 fade_new(vec2 t) {
    return t * t * t * ( t * (t*6-15) + 10 );
}

// Look-up in the permutation table at index idx [0,255].
int perm(int idx) {
    int idx_mod = int(mod(idx, 255));
    return int(texture(permTex1D, idx_mod/255.0, 0).r);
}

// Look-up in the gradient vectors table at index idx [0,3].
float grad(int idx, vec2 position) {
    int idx_mod = int(mod(idx, 8)); // With    zeros in gradients.
    vec2 gradients[] = vec2[](
            vec2(1.0f,  1.0f),
            vec2(-1.0f,  1.0f),
            vec2(1.0f, -1.0f),
            vec2(-1.0f, -1.0f),
            vec2(0.0f,  1.0f),
            vec2(0.0f, -1.0f),
            vec2(1.0f,  0.0f),
            vec2(-1.0f,  0.0f)
    );
    
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

    // Loop will be unrolled by the compiler (GPU driver).
    for (int k=0; k<octaves; k++) {
        height   += (perlin_noise(position) * pow(lacunarity, -H*k));
        position *= lacunarity;
    }
    return height;
}

void main() {
   height = vec3(perlin_noise(uv*10));
   //height = vec3(grad(3, vec2(0.5f, 0.f)));
}


