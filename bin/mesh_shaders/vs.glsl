#version 410 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 texcoord;

//flat out vec3 fs_normal;
out vec3 fs_normal;
out vec2 fs_texcoord;

uniform mat3 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;

void main() 
{
	fs_texcoord = texcoord;
	fs_normal = vNormal; //normalize(ModelViewMatrix * vNormal);
    gl_Position = ModelViewProjectionMatrix * vPosition;

 }