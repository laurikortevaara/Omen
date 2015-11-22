#version 410 core

layout(location = 0) in vec4 vPosition;
//layout(location = 1) in vec3 baryCentric;
layout(location = 1) in vec2 texcoord;

//out vec3 vBC;
out vec2 tc_texcoord;
out vec4 tc_position;

uniform mat4 ModelViewProjectionMatrix;

void main() 
{
	//vBC = baryCentric;
	tc_texcoord = texcoord;
	tc_position = vPosition;
    gl_Position = ModelViewProjectionMatrix * vPosition;
 }