#version 410 core

layout (quads, equal_spacing, ccw) in;

in  vec3 patch_vBC[];
out vec3 vBC;

in 	vec2 te_texcoord[];
out vec2 fs_texcoord;

in  vec4 teInPosition[];

uniform mat4 ModelViewProjectionMatrix;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
   	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
   	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
	vBC = (gl_TessCoord.x * patch_vBC[0] + gl_TessCoord.y * patch_vBC[1] + gl_TessCoord.z * patch_vBC[2]);
	fs_texcoord = (gl_TessCoord.x * te_texcoord[0] + gl_TessCoord.y * te_texcoord[1] + gl_TessCoord.z * te_texcoord[2]);

	float u = gl_TessCoord.x;
	float omu = 1 - u; // one minus "u"
	float v = gl_TessCoord.y;
	float omv = 1 - v; // one minus "v"
	
	gl_Position =
		gl_in[0].gl_Position * omu * omv  + 
		gl_in[1].gl_Position * u * omv  +
		gl_in[2].gl_Position * u * v + 
		gl_in[3].gl_Position * omu * v;
}