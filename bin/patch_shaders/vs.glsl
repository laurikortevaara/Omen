#version 410 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 barycentric;
layout(location = 2) in vec2 texcoord;

out vec3 tc_barycentric;
out vec2 tc_texcoord;
out vec4 tc_position;

uniform mat4 ModelViewProjectionMatrix;

void main() 
{
	tc_barycentric = barycentric;
	tc_texcoord = texcoord;
	tc_position = vPosition;

    gl_Position = ModelViewProjectionMatrix * vPosition;
 }