#version 410 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 texcoord;

out vec2 fs_texcoord;

uniform mat4 ModelViewProjectionMatrix;

void main() 
{
	fs_texcoord = texcoord;
    gl_Position = ModelViewProjectionMatrix * vPosition;
 }