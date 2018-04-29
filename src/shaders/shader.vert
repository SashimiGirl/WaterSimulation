#version 330 core

layout(location = 0) in vec3 position;
//layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec3 fPosition;
out vec3 fNormal;

void main() {
    fPosition = position;
    fNormal = normalize(normal);
    gl_Position = projectionMatrix * modelViewMatrix * modelMatrix * vec4(position, 1.0);
}
