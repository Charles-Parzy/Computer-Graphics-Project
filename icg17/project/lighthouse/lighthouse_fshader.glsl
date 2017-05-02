#version 330

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

out vec3 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

void main() {
    vec3 r = normalize(2*normal_mv*(max(0.0f, dot(normal_mv,light_dir))) - light_dir);
    vec3 ambiant = ka * La;
    vec3 diffuse = kd*(max(0.0f, dot(normal_mv, light_dir)))*Ld;
    vec3 specular = ks*pow((max(0.0f, dot(r, view_dir))),alpha)*Ls;

    color = ambiant + diffuse + specular;
}