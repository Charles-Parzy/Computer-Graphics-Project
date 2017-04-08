#version 330

in vec2 texture_coordinates;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

uniform vec3 La, Ld, Ls;
uniform sampler2D heightMap;

out vec3 color;

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
vec3 grassKa = vec3(0.0f, 0.67f, 0.0f);
vec3 grassKd = vec3(0.15f, 0.15f, 0.15f);
vec3 grassKs = vec3(0.0f, 0.0f, 0.0f);

/*************
SNOW values
**************/
vec3 snowKa = vec3(1.0f, 0.98f, 0.98f);
vec3 snowKd = vec3(0.0f, 0.0f, 0.0f);
vec3 snowKs = vec3(0.0f, 0.0f, 0.0f);

/*************
WATER values
**************/
vec3 waterKa = vec3(0.25f, 0.64f, 0.88f);
vec3 waterKd = vec3(0.1f, 0.1f, 0.1f);
vec3 waterKs = vec3(0.2f, 0.2f, 0.2f);

/*************
HEIGHT values
**************/
const float sandMin = 0.000f;
const float forestMin = 0.018f;
const float snowMin = 0.21f;

void main() {
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

	} else if (height > sandMin && height < forestMin) {
        ambiant = sandKa * La;
        diffuse = sandKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld;
        specular = sandKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls; 	

    } else {
        ambiant = waterKa * La;
        diffuse = waterKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld;
        specular = waterKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls;  
    }

    color = ambiant + diffuse + specular;
}
