#version 410 core

layout(location = 0) in vec4 vPosition;
uniform mat4 ModelViewProjectionMatrix;

void main() 
{
     gl_Position = ModelViewProjectionMatrix*vPosition;
 }