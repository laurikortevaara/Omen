#version 410 core

layout (triangles, equal_spacing, ccw) in;

in  vec3 patch_vBC[];
out vec3 vBC;

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
	//vBC = (patch_vBC[0] + patch_vBC[1] + patch_vBC[2]);

	gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position +
                   gl_TessCoord.y * gl_in[1].gl_Position +
                   gl_TessCoord.z * gl_in[2].gl_Position);
}