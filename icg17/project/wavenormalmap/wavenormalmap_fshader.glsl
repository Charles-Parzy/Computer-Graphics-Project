#version 330

in vec2 uv;

out vec3 height;
uniform sampler2D heightmap;
uniform int fps;

const float pi = 3.14159265359;

const int num_waves = 5;

vec3 gen_wave_height(vec2 new_uv, float height, int fps, int frame) {
  // defining the properties of all waves on screen
  // amplitude
  float time = 2 * pi * (float(frame)/float(fps));

  float a[num_waves] = float[](0.04f, //* sin(time) + 0.01f,
                               0.03f,
                               0.02f,
                               0.01f,
                               0.005f);
  // wavenumber
  float w[num_waves] = float[](1000.0f + 20 * height,//* texture(wave, uv).r,
                               1500.0f + 30 * height,//* (1 - texture(wave, 0.9 * uv).r),
                               1000.0f + 40 * height,
                               2000.0f + 50 * height,
                               2000.0f + 60 * height);


    // wave roundness
  float q[num_waves] = float[](0.5f,
                               0.6f,
                               0.7f,
                               0.8f,
                               0.9f);

  // wave speed
  float o[num_waves] = float[](1.0f,
                               1.0f,
                               1.0f,
                               1.0f,
                               1.0f);
  // direction
  vec2 d[num_waves] = vec2[](
    // vec2(1.0f + height/10 , 1.0f + height/2),
    // vec2(1.0f + height/8 , 1.0f + height/4),
    // vec2(1.0f + height/6 , 1.0f + height/6),
    // vec2(1.0f + height/4 , 1.0f + height/8),
    // vec2(1.0f + height/2 , 1.0f + height/10));

    //vec2(1.0f, 0.0f),//vec2(1.0f + height/10 , 1.0f + height/2),
    normalize(vec2(0.1f, 0.5f)),//vec2(1.0f + height/10 , 1.0f + height/2),
    normalize(vec2(1.2f, 1.4f)),//vec2(1.0f + height/8 , 1.0f + height/4),
    normalize(vec2(1.3f, 1.3f)),//vec2(1.0f + height/6 , 1.0f + height/6),
    normalize(vec2(1.4f, 1.2f)),//vec2(1.0f + height/4 , 1.0f + height/8),
    normalize(vec2(1.5f, 1.1f)));//vec2(1.0f + height/2 , 1.0f + height/10));

  // sending height to fshader so I can debug via color
  float temp = 0;
  float height_w = 0;
  float x_norm = 0;
  float y_norm = 0;
  float z_norm = 1;

  // iterating over all the waves and calculating the x, y and height for
  // a single point at uv
  for (int i = 0;  i < num_waves; i++) {
      temp = w[i] * (dot(new_uv, d[i]))+ (time * o[i]);
      //height_w += a[i] * 2 * pow((sin(temp) + a[i])/2, 1.5f);
      x_norm += w[i] * d[i][0] * a[i] * cos(temp) * sin(temp) * 0.53;
      y_norm += w[i] * d[i][1] * a[i] * cos(temp) * sin(temp) * 0.53;
      z_norm -= q[i] * w[i] * sin(temp) * cos(temp) * 0.53;
  }

  return vec3(x_norm, y_norm, z_norm);
}


void main() {
  int size = int(ceil(sqrt(float(60))));
  int col = int(ceil(uv.x / (1.0f / float(size))));
  int row = int((uv.y / (1.0f / float(size))));
  int frame = (size * row) + col;
  vec2 new_uv = mod(uv, 1.0f/size) / (1.0f / size);

  // mapping to clifford torus
  float nx = cos(new_uv.x*2*pi)/(2*pi);
  float ny = cos(new_uv.y*2*pi)/(2*pi);
  float nz = sin(new_uv.x*2*pi)/(2*pi);
  float nw = sin(new_uv.y*2*pi)/(2*pi);

  //float height_raw = hybridMultifractal(vec4(nx,ny,nz,nw), 0.25f, 2.3f, 15, 0.7f);
  float height_raw = texture(heightmap, new_uv).r;

  if (frame <= fps) {
    height = gen_wave_height(new_uv, height_raw, fps, frame);
  } else {
    height = vec3(0);
  }
}


