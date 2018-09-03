#version 410 core

in vec3 fs_barycentric;
in vec2 fs_texcoord;

out vec4 fColor;

uniform sampler2D tex;
uniform int useTexture; 

void main() 
{
	if(useTexture == 0)
		fColor = texture(tex,fs_texcoord);
	if(useTexture == 1)
		fColor = vec4(0,0,1,1);
	if(useTexture == 2)
		fColor = vec4(fs_texcoord.x*0.5, 0,0, 1);		
	/*if(useTexture == 3)
		fColor = vec4(fc_barycentric.x, fc_barycentric.y, fc_barycentric.z, 1);		*/


	if(any(lessThan(fs_barycentric, vec3(0.001)))){
    	fColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}