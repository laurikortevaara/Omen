#version 410

layout(triangles, equal_spacing) in;

// Input attributs
in vec3 WorldPos_ES_in[];

// Output attributs
out vec3 WorldPos_FS_in;

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)                                                   
{                                                                                               
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;   
}      

void main(void)
{
    WorldPos_FS_in = interpolate3D(WorldPos_ES_in[0],WorldPos_ES_in[1],WorldPos_ES_in[2]);
    gl_Position = vec4(WorldPos_FS_in, 1.0);   
}
