#version 410 core

in  vec3 vBC[];
out vec3 patch_vBC[];

in  vec4 tcInPosition[];
out vec4 teInPosition[];

layout (vertices = 3) out;

uniform float innerTess;
uniform float outerTess;

void main()
{
	patch_vBC[gl_InvocationID]=vBC[gl_InvocationID];
	teInPosition[gl_InvocationID]=tcInPosition[gl_InvocationID];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	
	// and then set tessellation levels
	gl_TessLevelInner[0] = innerTess;
	gl_TessLevelInner[1] = innerTess;

	gl_TessLevelOuter[0] = outerTess;
	gl_TessLevelOuter[1] = outerTess;
	gl_TessLevelOuter[2] = outerTess;
	
}