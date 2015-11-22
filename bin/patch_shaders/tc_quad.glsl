#version 410 core

in  vec3 tc_barycentric[];
out vec3 te_barycentric[];

in  vec4 tc_position[];
out vec4 te_position[];

in  vec2 tc_texcoord[];
out vec2 te_texcoord[];

layout (vertices = 4) out;

uniform float innerTess;
uniform float outerTess;

void main()
{
	te_barycentric[gl_InvocationID]=tc_barycentric[gl_InvocationID];
	te_position[gl_InvocationID]=tc_position[gl_InvocationID];
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