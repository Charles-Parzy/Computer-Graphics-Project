#version 330

in vec2 texture_coordinates;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;
in vec3 wavenormal;
in mat4 mv;

uniform vec3 La, Ld, Ls;
uniform bool isWater;
uniform bool isReflection;
uniform float heightmap_width;
uniform float heightmap_height;

//Texures
uniform sampler2D heightMap;
uniform sampler2D GrassTex2D;
uniform sampler2D RockTex2D;
uniform sampler2D SeabedTex2D;
uniform sampler2D SandTex2D;
uniform sampler2D SnowTex2D;
uniform sampler2D WaterTex2D;
uniform sampler2D reflection;

out vec4 color;

/*************
SAND values
**************/
vec3 sandKa = texture(SandTex2D, texture_coordinates).rgb;
vec3 sandKd = vec3(0.2f, 0.2f, 0.2f);
vec3 sandKs = vec3(0.0f, 0.0f, 0.0f);

/*************
GRASS values
**************/
vec3 grassKa = texture(GrassTex2D, texture_coordinates).rgb;
vec3 grassKd = vec3(0.15f, 0.15f, 0.15f);
vec3 grassKs = vec3(0.0f, 0.0f, 0.0f);

/*************
ROCK values
**************/
vec3 rockKa = texture(RockTex2D, texture_coordinates).rgb;
vec3 rockKd = vec3(0.2f, 0.2f, 0.2f);
vec3 rockKs = vec3(0.0f, 0.0f, 0.00f);

/*************
SNOW values
**************/
vec3 snowKa = texture(SnowTex2D, texture_coordinates).rgb;
vec3 snowKd = vec3(0.2f, 0.2f, 0.2f);
vec3 snowKs = vec3(0.2f, 0.2f, 0.2f);

/*************
Seabed values
**************/
vec3 underKa = texture(SeabedTex2D, texture_coordinates).rgb;
vec3 underKd = vec3(0.2f, 0.2f, 0.2f);
vec3 underKs = vec3(0.0f, 0.0f, 0.0f);

/*************
WATER COLOR
**************/
//vec3 waterKa = texture(WaterTex2D, texture_coordinates).rgb;
vec3 waterKa = vec3(0.0f, 0.6f, 0.6f);
vec3 waterKd = vec3(0.0f, 0.31f, 0.31f);
vec3 waterKs = vec3(0.0f, 0.0f, 0.0f);

/*************
CONSTANT values
**************/
//const float default_alpha = 1.0f;
const float default_alpha = 60.0f;
const float sandMin = 0.130f;
const float forestMin = 0.135f;
const float snowMin = 0.26f;
const float rockMin = 0.18f;
const float epsilon = 0.02f;

vec3 getWaterColor(float percentageDarkBlue) {
    return mix(waterKa, vec3(0.0f, 0.0f, 0.0f), vec3(percentageDarkBlue));
}

void main() {
    float height = texture(heightMap, texture_coordinates).r;
    
    /// TODO: query window_width/height using the textureSize(..) function on tex_mirror
    float window_width = textureSize(reflection, 0).x;
    float window_height = textureSize(reflection, 0).y;
    /// TODO: use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
    float _u = gl_FragCoord.x/window_width;
    float _v = gl_FragCoord.y/window_height;

    vec3 ambiant;
    vec3 diffuse;
    vec3 specular;

    if(isWater) {
        vec3 mirrornormal = vec3(0,0,1);
        vec3 x = dFdx(vpoint_mv).xyz;
        vec3 y = dFdy(vpoint_mv).xyz;
        vec3 normal_mv = normalize(cross(x,y));
        vec3 r = normalize(2*normal_mv*(max(0.0f, dot(normal_mv,light_dir))) - light_dir);

        ambiant = waterKa * La;
        diffuse = waterKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld;
        specular = waterKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls;

        //vec3 flatnormal = wavenormal - dot(wavenormal, mirrornormal) * mirrornormal;
        //vec3 eyenormal = transpose(inverse(mat3(mv))) * flatnormal;
        //vec2 offset = normalize(eyenormal.xy) * length(flatnormal) * 0.1; 

        if(height >= sandMin && height <= sandMin + 0.005) {
            float waterPerc = 0.7f - 0.7f * ((height - (sandMin)) / 0.005f);
            //color = vec4(ambiant + diffuse + specular, waterPerc);
            color = vec4(mix(ambiant + diffuse + specular, texture(reflection,vec2(_u,_v)).rgb, 0.5), waterPerc);
        } else if (height < sandMin){
            height = max(0.0f, height);
            float percentageDarkBlue = (sandMin-height)/sandMin;
            ambiant = getWaterColor(percentageDarkBlue) * La;
            color = vec4(mix(ambiant + diffuse + specular, texture(reflection,vec2(_u,_v)).rgb, 0.5), 0.7f);
            //color = vec4(ambiant + diffuse + specular, 0.7f);
        } else {
            color = vec4(1.0f, 1.0f, 1.0f, 0.0f);
        }
        //color = vec4(mix(color.rgb, texture(reflection,vec2(_u,_v)).rgb, 1.0), 1.0);
    } else {
        vec2 x1_temp = vec2(texture_coordinates.x-(1.0/heightmap_width), texture_coordinates.y);
        vec2 x2_temp = vec2(texture_coordinates.x+(1.0/heightmap_width), texture_coordinates.y);

        vec3 x1 = vec3(x1_temp, texture(heightMap, x1_temp).r);
        vec3 x2 = vec3(x2_temp, texture(heightMap, x2_temp).r);

        vec2 y1_temp = vec2(texture_coordinates.x, texture_coordinates.y-(1.0/heightmap_height));
        vec2 y2_temp = vec2(texture_coordinates.x, texture_coordinates.y+(1.0/heightmap_height));

        vec3 y1 = vec3(y1_temp, texture(heightMap, y1_temp).r);
        vec3 y2 = vec3(y2_temp, texture(heightMap, y2_temp).r);

        vec3 x = normalize(x2-x1);
        vec3 y = normalize(y2-y1);   
        vec3 normal_mv = normalize(cross(x,y));
        vec3 r = normalize(2*normal_mv*(max(0.0f, dot(normal_mv,light_dir))) - light_dir);

        if (height >= snowMin + epsilon) { // Only white snow
            ambiant = snowKa * La;
            diffuse = snowKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld;
            specular = snowKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls; 
        
        } else if (height > snowMin) { // Gradient white snow and grey rock
            float percentageGrey = ((snowMin + epsilon) - height)/epsilon;
            float percentageWhite = 1.0 - percentageGrey;

            ambiant = (percentageWhite * snowKa * La) + (percentageGrey* rockKa * La);
            diffuse = (percentageWhite * snowKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld) + (percentageGrey*rockKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld);
            specular = (percentageWhite*snowKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls) + (percentageGrey*rockKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls);  

        } else if (height > rockMin + epsilon) { // Only grey rock
            ambiant  = rockKa * La;
            diffuse = rockKd * (max(0.0f, dot(normal_mv, light_dir))) * Ld;
            specular = rockKs * pow((max(0.0f, dot(r, view_dir))),default_alpha) * Ls; 

        } else if (height > rockMin) { // Gradient grey rock and grass
            float percentageGreen = ((rockMin + epsilon) - height)/epsilon;
            float percentageGrey = 1.0 - percentageGreen;

            ambiant = (percentageGrey * rockKa * La) + (percentageGreen* grassKa * La);
            diffuse = (percentageGrey * rockKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld) + (percentageGreen*grassKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld);
            specular = (percentageGrey*rockKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls) + (percentageGreen*grassKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls); 

        } else if (height >= forestMin + epsilon) { // Only Grass
            ambiant = grassKa * La;
            diffuse = grassKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld;
            specular = grassKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls; 

        } else if (height >= forestMin) { // Gradient grass and sand
            float percentageSand = ((forestMin + epsilon) - height)/epsilon;
            float percentageGreen = 1.0 - percentageSand;

            ambiant = (percentageGreen * grassKa * La) + (percentageSand* sandKa * La);
            diffuse = (percentageGreen * grassKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld) + (percentageSand*sandKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld);
            specular = (percentageGreen*grassKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls) + (percentageSand*sandKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls);  

        } else if (height >= sandMin) { // Only sand
            ambiant = sandKa * La;
            diffuse = sandKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld;
            specular = sandKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls;

        } else { // Only underwater
            ambiant = underKa * La;
            diffuse = underKd*(max(0.0f, dot(normal_mv, light_dir)))*Ld;
            specular = underKs*pow((max(0.0f, dot(r, view_dir))),default_alpha)*Ls;  
        }

        color = vec4(ambiant + diffuse + specular, 1.0f);
    }

    if(isReflection) {
       if (height < sandMin) {
        color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
       } 
    }

}