#version 410 core

layout (vertices = 4) out;
void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	
	// and then set tessellation levels
	gl_TessLevelOuter[0] = 2.0;
	gl_TessLevelOuter[1] = 3.0;
	gl_TessLevelOuter[2] = 2.0;
	gl_TessLevelOuter[3] = 5.0;
	gl_TessLevelInner[0] = 3.0;
	gl_TessLevelInner[1] = 4.0;
}