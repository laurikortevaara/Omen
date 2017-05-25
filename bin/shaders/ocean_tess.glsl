#ifdef TESS_CONTROL_SHADER
//-- TessControl

layout(vertices = 3) out;

in vec3 VPosition[];
in vec3 FacetNormal[];
in vec3 PatchDistance[];
in vec3 TriDistance[];
in vec2 TCoord[];
in vec4 pos_eye[];
in vec3 normal_eye[];

out vec3 tcPosition[];
out vec2 tcTCoord[];
out vec4 tc_pos_eye[];
out vec3 tc_normal_eye[];

#define ID gl_InvocationID

void main()
{
    tcPosition[ID] = VPosition[ID];
	tcTCoord[ID] = TCoord[ID];
	tc_pos_eye[ID] = pos_eye[ID];
	tc_normal_eye[ID] = normal_eye[ID];

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
in vec4 tc_pos_eye[];
in vec3 tc_normal_eye[];

out vec3 tePosition;
out vec3 tePatchDistance;
out vec2 teTCoord;
out vec3 teNormal;
out vec4 te_pos_eye;
out vec3 te_normal_eye;

uniform mat4 Projection;
uniform mat4 Modelview;
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

	vec4 pe0 = gl_TessCoord.x * tc_pos_eye[0];
	vec4 pe1 = gl_TessCoord.y * tc_pos_eye[1];
	vec4 pe2 = gl_TessCoord.z * tc_pos_eye[2];

	vec3 ne0 = gl_TessCoord.x * tc_normal_eye[0];
	vec3 ne1 = gl_TessCoord.y * tc_normal_eye[1];
	vec3 ne2 = gl_TessCoord.z * tc_normal_eye[2];
	
    tePosition = p0 + p1 + p2;
	teTCoord = t0 + t1 + t2;

	te_pos_eye = pe0 + pe1 + pe2;
	te_normal_eye = ne0 + ne1 + ne2;

	//teTCoord*=2;

	tePosition += texture(Texture, teTCoord).xyz;

	
	/*
	float f = 1.0;
	vec4 color = vec4(0);
	for(int i=0; i < 10; ++i)
	{
		color += vec4(0, f*perlin(i*(teTCoord)*PerlinScale,PerlinSize)*0.5+0.5,0,1);
		f /= 2.0;
	}
	//color = normalize(color)*2.0-1.0;
	//color /= 10;

	//tePosition += gl_TessLevelOuter[3]*color.xyz*sin(iGlobalTime);
	*/
	teNormal = normalize(texture(Texture2, teTCoord).xyz);
	
	te_pos_eye = vec4(ViewMatrix*ModelMatrix*vec4(tePosition,1));
	te_normal_eye = vec3(ViewMatrix*ModelMatrix*vec4(teNormal,1));

	gl_Position = vec4(tePosition, 1);
}
#endif

#ifdef GEOMETRY_SHADER
//-- Geometry
uniform mat3 NormalMatrix;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tePosition[3];
in vec3 tePatchDistance[3];
in vec2 teTCoord[3];
in vec3 teNormal[3];
in vec4 te_pos_eye[3];
in vec3 te_normal_eye[3];

smooth out vec3 FacetNormal;
out vec3 Position;
out vec3 PatchDistance;
out vec3 TriDistance;
out vec3 Normal;
out vec2 TCoord;
out vec4 pos_eye;
out vec3 normal_eye;

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
	pos_eye = te_pos_eye[0];
	normal_eye = te_normal_eye[0];
    gl_Position = ModelViewProjection*gl_in[0].gl_Position; EmitVertex();

    PatchDistance = tePatchDistance[1];
	TCoord = teTCoord[1];
    TriDistance = vec3(0, 1, 0);
	Normal = teNormal[1];
	Position = tePosition[1];
	pos_eye = te_pos_eye[1];
	normal_eye = te_normal_eye[2];
    gl_Position = ModelViewProjection*gl_in[1].gl_Position; EmitVertex();

    PatchDistance = tePatchDistance[2];
    TCoord = teTCoord[2];
	TriDistance = vec3(0, 0, 1);
	Normal = teNormal[2];
	Position = tePosition[2];
	pos_eye = te_pos_eye[2];
	normal_eye = te_normal_eye[2];
    gl_Position = ModelViewProjection*gl_in[2].gl_Position; EmitVertex();

    EndPrimitive();
}

#endif
