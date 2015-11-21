#version 410 core

in  vec3 vBC[];
out vec3 patch_vBC[];

in  vec4 tcInPosition[];
out vec4 teInPosition[];

in  vec2 tc_texcoord[];
out vec2 te_texcoord[];

layout (vertices = 4) out;

uniform float innerTess;
uniform float outerTess;

void main()
{
	patch_vBC[gl_InvocationID]=vBC[gl_InvocationID];
	teInPosition[gl_InvocationID]=tcInPosition[gl_InvocationID];
	te_texcoord[gl_InvocationID]=tc_texcoord[gl_InvocationID];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	
	// and then set tessellation levels
	gl_TessLevelInner[0] = innerTess;
	gl_TessLevelInner[1] = innerTess;

	gl_TessLevelOuter[0] = outerTess;
	gl_TessLevelOuter[1] = outerTess;
	gl_TessLevelOuter[2] = outerTess;
	gl_TessLevelOuter[3] = outerTess;
	
}