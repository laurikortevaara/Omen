#version 410 core

in vec3 vBC;
in vec2 fs_texcoord;

out vec4 fColor;

uniform sampler2D tex;

void main() 
{
	if(any(lessThan(vBC, vec3(0.002)))){
    	fColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else{
    	fColor = texture(tex, fs_texcoord);
	}
	fColor = texture(tex, fs_texcoord);
	//fColor = vec4(vBC,1);
}