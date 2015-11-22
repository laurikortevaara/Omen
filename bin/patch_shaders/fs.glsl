#version 410 core

//in vec3 vBC;
in vec2 fs_texcoord;

out vec4 fColor;

uniform sampler2D tex;

void main() 
{
	fColor = texture(tex,fs_texcoord);
	//fColor = vec4(fs_texcoord.y, 0, 0, 1);
}