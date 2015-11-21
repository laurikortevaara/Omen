// This is a very simple pass-through geometry shader
#version 410 core

// Specify the input and output primitive types, along
// with the maximum number of vertices that this shader
// might produce. Here, the input type is triangles and
// the output type is triangle strips.

layout (quads) in;
layout (quads, max_vertices = 4) out;

// Geometry shaders have a main function just
// like any other type of shader

void main()
{
	int n;
	// Loop over the input vertices
	for (n = 0; n < gl_in.length(); n++)
	{
	// Copy the input position to the output
	gl_Position = gl_in[0].gl_Position;
	// Emit the vertex
	EmitVertex();
	}
	// End the primitive. This is not strictly necessary
	// and is only here for illustrative purposes.
	EndPrimitive();
}