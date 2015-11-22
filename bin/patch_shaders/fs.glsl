#version 410 core

in vec3 fc_barycentric;
in vec2 fs_texcoord;

out vec4 fColor;

uniform sampler2D tex;

void main() 
{
	fColor = texture(tex,fs_texcoord);

	fColor = vec4(1,1,1,1);

/*
	if(any(lessThan(fc_barycentric, vec3(0.02)))){
    	fColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else{
    	fColor = vec4(0.5, 0.5, 0.5, 1.0);
	}
	*/
}