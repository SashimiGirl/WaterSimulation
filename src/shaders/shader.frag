#version 330 core
layout(location = 0) out vec4 frag_color;
precision highp float;

uniform vec3 lPosition;
uniform vec3 lIntensity;

in vec3 fPosition;
in vec3 fNormal;

void main() {
    //float len = length(lPosition - fPosition);
    vec3 ambient = vec3(0.3, 0.3, 0.3);
    vec3 diffuse = lIntensity * max(0.0, dot(normalize(lPosition), fNormal));
    frag_color = vec4(ambient + diffuse, 1.0);
}
