#version 410 core

layout (triangles, equal_spacing, ccw) in;

in  vec3 te_barycentric[];
out vec3 fs_barycentric;

in 	vec2 te_texcoord[];
out vec2 fs_texcoord;

in  vec4 te_position[];

uniform sampler2D tex;
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
	fs_barycentric = interpolate3D(te_barycentric[0],te_barycentric[1],te_barycentric[2]);
    fs_texcoord = interpolate2D(te_texcoord[0], te_texcoord[1], te_texcoord[2]);

	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

	float omu = 1 - u; // one minus "u"
	float omv = 1 - v; // one minus "v"

	vec3 c = texture(tex,fs_texcoord).xyz;
	float height = (c.x+c.y+c.z)/3;
    //vec4 displaced = vec4( vPosition.x, vPosition.y, height, 1.0);

	gl_Position = ((gl_TessCoord.x) * gl_in[0].gl_Position +
                   (gl_TessCoord.y) * gl_in[1].gl_Position +
	               (gl_TessCoord.z) * gl_in[2].gl_Position);


	gl_Position.y = gl_Position.y + height*25;
}