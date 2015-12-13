#version 410 core

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out Data{
    vec3 normal;
    vec4 eye;
    vec2 texcoord;
} DataOut;


void main() 
{
    DataOut.normal = normalize(NormalMatrix * normal);
    DataOut.eye = -(ModelViewMatrix * position);

    gl_Position = ModelViewProjectionMatrix * position;

 }