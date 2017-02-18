#ifdef TESS_CONTROL_SHADER
layout(vertices = 3) out;

in vec3 vPosition[];
out vec3 tcPosition[];

#define ID gl_InvocationID

float GetTessLevel(float Distance0, float Distance1)
{
	return 5;
    float AvgDistance = (Distance0 + Distance1) / 2.0;

    if (AvgDistance <= 20.0) {
        return 20.0-AvgDistance;
		}
    else {
        return 1.0;
    }
} 

void main(void)
{
    // Only if I am invocation 0 ...
    if (gl_InvocationID == 0)
    {
        // Calculate the distance from the camera to the three control points
		float EyeToVertexDistance0 = distance(gEyeWorldPos, vPosition[0]);
		float EyeToVertexDistance1 = distance(gEyeWorldPos, vPosition[1]);
		float EyeToVertexDistance2 = distance(gEyeWorldPos, vPosition[2]);

		// Calculate the tessellation levels
		gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
		gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
		gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
		gl_TessLevelInner[0] = gl_TessLevelOuter[2];
    }

    // Everybody copies their input to their output
    gl_out[ID].gl_Position = gl_in[ID].gl_Position;
	tcPosition[ID] = vPosition[ID];
}

#endif

#ifdef TESS_EVALUATION_SHADER
layout(triangles, equal_spacing, ccw) in;
in vec3 tcPosition[];

out vec3 tePosition;
out vec3 teFacetNormal;

void main()
{
    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];
        
	gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position +
                   gl_TessCoord.y * gl_in[1].gl_Position +
                   gl_TessCoord.z * gl_in[2].gl_Position);

	tePosition = normalize(p0+p1+p2);
}
#endif