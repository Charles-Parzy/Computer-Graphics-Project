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

  float a[num_waves] = float[](0.004f, //* sin(time) + 0.01f,
                               0.003f,
                               0.002f,
                               0.001f,
                               0.0005f);
  // wavenumber
  float w[num_waves] = float[](5000.0f + 200 * height,//* texture(wave, uv).r,
                               7500.0f + 300 * height,//* (1 - texture(wave, 0.9 * uv).r),
                               5000.0f + 400 * height,
                               10000.0f + 500 * height,
                               10000.0f + 600 * height);


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
  float height_w = 0;
  float temp = 0;
  float new_y = new_uv.y;
  float new_x = new_uv.x;

  // iterating over all the waves and calculating the x, y and height for
  // a single point at uv
  for (int i = 0;  i < num_waves; i++) {
      temp = w[i] * (dot(new_uv, d[i]))+ (time * o[i]);
      height_w += a[i] * 2 * pow((sin(temp) + a[i])/2, 1.5f);
      new_x += a[i] * q[i] * d[i][0] * pow((cos(temp) + a[i])/2, 1.5f);
      new_y += a[i] * q[i] * d[i][1] * pow((cos(temp) + a[i])/2, 1.5f);
  }
  return vec3(new_x, new_y, height_w/0.0105f);
}


void main() {
  int size = int(ceil(sqrt(float(fps))));
  int col = int(ceil(uv.x / (1.0f / float(size))));
  int row = int((uv.y / (1.0f / float(size))));
  int frame = (size * row) + col;
  vec2 new_uv = mod(uv, 1.0f/float(size)) / (1.0f / float(size));

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

  //height = vec3(height_raw);
  //height = vec3(new_uv.x);
}


