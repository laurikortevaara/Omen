#ifdef TESS_CONTROL_SHADER
//-- TessControl

layout(vertices = 3) out;

in vec3 VPosition[];
in vec3 FacetNormal[];
in vec3 PatchDistance[];
in vec3 TriDistance[];
in vec2 TCoord[];

out vec3 tcPosition[];
out vec2 tcTCoord[];

#define ID gl_InvocationID

void main()
{
    tcPosition[ID] = VPosition[ID];
	tcTCoord[ID] = TCoord[ID];

    if (ID == 0) {
        gl_TessLevelInner[0] = InnerTessellationLevel1;
		gl_TessLevelInner[1] = InnerTessellationLevel2;
        gl_TessLevelOuter[0] = OuterTessellationLevel1;
        gl_TessLevelOuter[1] = OuterTessellationLevel2;
        gl_TessLevelOuter[2] = OuterTessellationLevel3;
		gl_TessLevelOuter[3] = OuterTessellationLevel4;
    }
}
#endif

#ifdef TESS_EVALUATION_SHADER
//-- TessEval

layout(triangles, equal_spacing, cw) in;

in vec3 tcPosition[];
in vec2 tcTCoord[];

out vec3 tePosition;
out vec3 tePatchDistance;
out vec2 teTCoord;
out vec3 teNormal;

uniform mat4 Projection;
uniform mat4 Modelview;
uniform mat4 ViewMatrix;
uniform mat3 NormalMatrix;

void main()
{
    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];
    tePatchDistance = gl_TessCoord;

	vec2 t0 = gl_TessCoord.x * tcTCoord[0];
	vec2 t1 = gl_TessCoord.y * tcTCoord[1];
	vec2 t2 = gl_TessCoord.z * tcTCoord[2];
	
    tePosition = p0 + p1 + p2;
	teTCoord = t0 + t1 + t2;

	tePosition += texture(Texture, teTCoord).xyz;
	teNormal = normalize(texture(Texture2, teTCoord).xyz);
	
	gl_Position = vec4(tePosition, 1);
}
#endif

#ifdef GEOMETRY_SHADER
//-- Geometry

uniform mat4 ViewMatrix;
uniform mat3 NormalMatrix;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tePosition[3];
in vec3 tePatchDistance[3];
in vec2 teTCoord[3];
in vec3 teNormal[3];

smooth out vec3 FacetNormal;
out vec3 Position;
out vec3 PatchDistance;
out vec3 TriDistance;
out vec3 Normal;
out vec2 TCoord;

void main()
{
    vec3 A = (gl_in[1].gl_Position-gl_in[0].gl_Position).xyz;
    vec3 B = (gl_in[2].gl_Position-gl_in[0].gl_Position).xyz;
	mat4 normalMatrix = transpose((ViewMatrix));
    FacetNormal = normalize(cross(A, B));

    PatchDistance = tePatchDistance[0];
	TCoord = teTCoord[0];
    TriDistance = vec3(1, 0, 0);
	Normal = teNormal[0];
	Position = tePosition[0];
    gl_Position = ModelViewProjection*gl_in[0].gl_Position; EmitVertex();

    PatchDistance = tePatchDistance[1];
	TCoord = teTCoord[1];
    TriDistance = vec3(0, 1, 0);
	Normal = teNormal[1];
	Position = tePosition[1];
    gl_Position = ModelViewProjection*gl_in[1].gl_Position; EmitVertex();

    PatchDistance = tePatchDistance[2];
    TCoord = teTCoord[2];
	TriDistance = vec3(0, 0, 1);
	Normal = teNormal[2];
	Position = tePosition[2];
    gl_Position = ModelViewProjection*gl_in[2].gl_Position; EmitVertex();

    EndPrimitive();
}

#endif
