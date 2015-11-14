#version 410 core

layout(vertices = 3) out;

// Attributes in
in vec3 WorldPos_CS_in[];

// Attributes out
out vec3 WorldPos_ES_in[];


void main(void)
{
    WorldPos_ES_in[gl_InvocationID] = WorldPos_CS_in[gl_InvocationID];
	
	if(gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = 1;
		gl_TessLevelOuter[0] = 1;
		gl_TessLevelOuter[1] = 1;
		gl_TessLevelOuter[2] = 1;
	}
}
