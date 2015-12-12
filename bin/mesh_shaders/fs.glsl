#version 410 core

in vec2 fs_texcoord;

out vec4 fColor;

uniform sampler2D tex;

void main() 
{
    fColor = vec4(1,1,1,1);
	//fColor = texture(tex, fs_texcoord);
	//fColor = (fColor+vec4(fs_texcoord.x, fs_texcoord.y, 0, 1))/2;
}