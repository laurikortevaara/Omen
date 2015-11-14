#version 410 core

in vec3 vBC;

out vec4 fColor;

void main() 
{
	if(any(lessThan(vBC, vec3(0.002)))){
    	fColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else{
    	fColor = vec4(0.5, 0.5, 0.5, 1.0);
	}
	fColor = vec4(vBC,1);
}