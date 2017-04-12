#version 330

in vec2 texture_coordinates;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

uniform vec3 La, Ld, Ls;
uniform sampler2D heightMap;

uniform bool isWater;

out vec4 color;

/*************
 SAND values
 **************/
vec3 sandKa = vec3(0.96f, 0.91, 0.40f);
vec3 sandKd = vec3(0.2f, 0.2f, 0.2f);
vec3 sandKs = vec3(0.0f, 0.0f, 0.0f);

/*************
 GRASS values
 **************/
vec3 grassKa = vec3(0.60f, 0.75f, 0.25f);
vec3 grassKd = vec3(0.30f, 0.40f, 0.13f);
vec3 grassKs = vec3(0.0f, 0.0f, 0.0f);

/*************
 FOREST values
 **************/
vec3 forestKa = vec3(0.30f, 0.40f, 0.13f);
vec3 forestKd = vec3(0.15f, 0.20f, 0.06f);
vec3 forestKs = vec3(0.0f, 0.0f, 0.0f);

/*************
 SNOW values
 **************/
vec3 snowKa = vec3(0.86f, 1.0f, 0.93f);
vec3 snowKd = vec3(0.2f, 0.2f, 0.2f);
vec3 snowKs = vec3(0.0f, 0.0f, 0.0f);

/*************
 UNDERGROUND values
 **************/
vec3 underKa = vec3(0.42f, 0.32f, 0.11f);
vec3 underKd = vec3(0.2f, 0.2f, 0.2f);
vec3 underKs = vec3(0.0f, 0.0f, 0.0f);

/*************
 WATER COLOR
 **************/
vec3 waterKa = vec3(0.0f, 0.74f, 0.74f);
vec3 waterKd = vec3(0.0f, 0.31f, 0.31f);
vec3 waterKs = vec3(0.0f, 0.0f, 0.0f);

/*************
 CONSTANT values
 **************/
const float default_alpha = 1.0f;
const float sandMin = 0.132f;
const float grassMin = 0.14f;
const float forestMin = 0.15f;
const float snowMin = 0.24f;
const float epsilonB = 0.02f;
const float epsilonS = 0.005f;

vec3[3] calcAmbDifSpe(vec3 ambdifspe[3], vec3 ka, vec3 kd, vec3 ks, float perc, vec3 norm_mv, vec3 r);

void main() {
    vec3 x = dFdx(vpoint_mv).xyz;
    vec3 y = dFdy(vpoint_mv).xyz;
    vec3 normal_mv = normalize(cross(x,y));
    vec3 r = normalize(2*normal_mv*(max(0.0f, dot(normal_mv,light_dir))) - light_dir);
    
    float height = texture(heightMap, texture_coordinates).r;
    
    vec3 ambdifspe[3];
    
    if(isWater) {
        ambdifspe = calcAmbDifSpe(ambdifspe, waterKa, waterKd, waterKs, 1.0, normal_mv, r);
        if(height >= sandMin && height <= sandMin + 0.005) {
            float waterPerc = 0.7f - 0.7f * ((height - (sandMin)) / 0.005f);
            color = vec4(ambdifspe[0] + ambdifspe[1] + ambdifspe[2], waterPerc);
        }else if (height < sandMin){
            color = vec4(ambdifspe[0] + ambdifspe[1] + ambdifspe[2], 0.7f);
        } else {
            color = vec4(1.0f, 1.0f, 1.0f, 0.0f);
        }
    } else {
        
        if (height >= snowMin + epsilonB) { // snow caps
            ambdifspe = calcAmbDifSpe(ambdifspe, snowKa, snowKd, snowKs, 1.0, normal_mv, r);
        
        } else if (height > snowMin) { // forest to snow
            float forestPerc = ((snowMin + epsilonB) - height) / epsilonB;
            float snowPerc = 1.0 - forestPerc;

            ambdifspe = calcAmbDifSpe(ambdifspe, snowKa, snowKd, snowKs, snowPerc, normal_mv, r);
            ambdifspe = calcAmbDifSpe(ambdifspe, forestKa, forestKd, forestKs, forestPerc, normal_mv, r);

        } else if (height >= (forestMin + epsilonB) && height < snowMin) { // forest
            ambdifspe = calcAmbDifSpe(ambdifspe, forestKa, forestKd, forestKs, 1.0, normal_mv, r);

        } else if (height >= forestMin && height < (forestMin + epsilonB)) { // forest to grass
            float grassPerc = ((forestMin + epsilonB) - height) / epsilonB;
            float forestPerc = 1.0 - grassPerc;

            ambdifspe = calcAmbDifSpe(ambdifspe, forestKa, forestKd, forestKs, forestPerc, normal_mv, r);
            ambdifspe = calcAmbDifSpe(ambdifspe, grassKa, grassKd, grassKs, grassPerc, normal_mv, r);
            
        } else if (height >= (grassMin + epsilonS) && height < forestMin) { // grass
            ambdifspe = calcAmbDifSpe(ambdifspe, grassKa, grassKd, grassKs, 1.0, normal_mv, r);
            
        } else if (height >= grassMin && height < (grassMin + epsilonS)) { // grass to sand
            float sandPerc = ((grassMin + epsilonS) - height) / epsilonS;
            float grassPerc = 1.0 - sandPerc;
            
            ambdifspe = calcAmbDifSpe(ambdifspe, grassKa, grassKd, grassKs, grassPerc, normal_mv, r);
            ambdifspe = calcAmbDifSpe(ambdifspe, sandKa, sandKd, sandKs, sandPerc, normal_mv, r);

    	} else if (height >= (sandMin + epsilonS) && height < grassMin) { // sand
            ambdifspe = calcAmbDifSpe(ambdifspe, sandKa, sandKd, sandKs, 1.0, normal_mv, r);

        } else if (height >= sandMin && height < (sandMin + epsilonS)) { // sand to dirt
            float underPerc = ((sandMin + epsilonS) - height) / epsilonS;
            float sandPerc = 1.0 - underPerc;
            
            ambdifspe = calcAmbDifSpe(ambdifspe, sandKa, sandKd, sandKs, sandPerc, normal_mv, r);
            ambdifspe = calcAmbDifSpe(ambdifspe, underKa, underKd, underKs, underPerc, normal_mv, r);
    
        } else {
            ambdifspe = calcAmbDifSpe(ambdifspe, underKa, underKd, underKs, 1.0, normal_mv, r);
        }
        
        //const vec3 RimColor = vec3(0.2, 0.2, 0.2);
        
        //const vec3 fogColor = vec3(0.5, 0.5,0.5);
        //const float FogDensity = 0.05;
        
        //rim lighting
        //float rim = 1 - max(dot(view_dir, normal_mv), 0.0);
        //rim = smoothstep(0.6, 1.0, rim);
        //vec3 finalRim = RimColor * vec3(rim, rim, rim);
        //get all lights and texture
        //vec3 lightColor = finalRim + ambdifspe[1] + ambdifspe[0] + ambdifspe[1] + ambdifspe[2];
        
        //vec3 finalColor = vec3(0, 0, 0);
        
        //compute range based distance
        //float dist = length(vpoint_mv);
        
        //my camera y is 10.0. you can change it or pass it as a uniform
        //float be = (10.0 - viewSpace.y) * 0.004;//0.004 is just a factor; change it if you want
        //float bi = (10.0 - viewSpace.y) * 0.001;//0.001 is just a factor; change it if you want
        
        //OpenGL SuperBible 6th edition uses a smoothstep function to get
        //a nice cutoff here
        //You have to tweak this values
        //float be = 0.025 * smoothstep(0.0, 1.0, 1.0 - vpoint_mv.y);
        //float bi = 0.075* smoothstep(0.0, 1.0, 1.0 - vpoint_mv.y);
        
        //float ext = exp(-dist * be);
        //float insc = exp(-dist * bi);
        
        //finalColor = fogColor * (1 - insc);//lightColor * ext
        
        //color = vec4(finalColor, 1.0f);
        color = vec4(ambdifspe[0] + ambdifspe[1] + ambdifspe[2], 1.0f);
    }
}

vec3[3] calcAmbDifSpe(vec3 ambdifspe[3], vec3 ka, vec3 kd, vec3 ks, float perc, vec3 norm_mv, vec3 r) {
    ambdifspe[0] += perc * ka * La;
    ambdifspe[1] += perc * kd * (max(0.0f, dot(norm_mv, light_dir))) * Ld;
    ambdifspe[2] += perc * ks * pow((max(0.0f, dot(r, view_dir))), default_alpha) * Ls;
    return ambdifspe;
}
