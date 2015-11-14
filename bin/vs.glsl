#version 410 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 baryCentric;

out vec3 vBC;
out vec4 tcInPosition;

uniform mat4 ModelViewProjectionMatrix;

void main() 
{
	vBC = baryCentric;
	tcInPosition = vPosition;
    gl_Position = ModelViewProjectionMatrix * vPosition;
 }