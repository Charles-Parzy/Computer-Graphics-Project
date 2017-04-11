#version 330

in vec2 texture_coordinates;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

uniform vec3 La, Ld, Ls;
uniform sampler2D heightMap;

uniform bool isWater;

out vec4 color;

float default_alpha = 1.0f;

/*************
SAND values
**************/
vec3 sandKa = vec3(0.73f, 0.67, 0.43f);
vec3 sandKd = vec3(0.2f, 0.2f, 0.2f);
vec3 sandKs = vec3(0.0f, 0.0f, 0.0f);

/*************
GRASS values
**************/
vec3 grassKa = vec3(0.0f, 0.55f, 0.0f);
vec3 grassKd = vec3(0.15f, 0.15f, 0.15f);
vec3 grassKs = vec3(0.0f, 0.0f, 0.0f);

/*************
SNOW values
**************/
vec3 snowKa = vec3(0.85f, 0.85f, 1.0f);
vec3 snowKd = vec3(0.2f, 0.2f, 0.2f);
vec3 snowKs = vec3(0.0f, 0.0f, 0.0f);

/*************
WATER values
**************/
vec3 water = vec3(0.0f, 0.0f, 1.0f);

/*************
HEIGHT values
**************/
const float sandMin = 0.132f;
const float forestMin = 0.14f;
const float snowMin = 0.24f;

void main() {

    if(isWater) {
        float height = texture(heightMap, texture_coordinates).r;
        if(height >= sandMin) {
            color = vec4(1.0f, 1.0f, 1.0f, 0.0f);
        }else {
            color = vec4(water, 0.5f);
        }
    } else {
        vec3 x = dFdx(vpoint_mv).xyz;
        vec3 y = dFdy(vpoint_mv).xyz;
        vec3 normal_mv = normalize(cross(x,y));
        vec3 r = normalize(2*normal_mv*(max(0.0f, dot(normal_mv,light_dir))) - light_dir);
    	
        float height = texture(heightMap, texture_coordinates).r;

        vec3 ambiant;
        vec3 diffuse;
        vec3 specular;

    	if (height >= snowMin) {
            ambiant = snowKa * La;
            diffuse = snowKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld;
            specular = snowKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls;	

        } else if (height >= forestMin && height < snowMin) {
            ambiant = grassKa * La;
            diffuse = grassKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld;
            specular = grassKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls; 

    	} else {
            ambiant = sandKa * La;
            diffuse = sandKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld;
            specular = sandKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls; 	

        }

        vec3 temp = vec3(ambiant + diffuse + specular);
        color = vec4(temp.xyz, 1.0f);
    }
}
