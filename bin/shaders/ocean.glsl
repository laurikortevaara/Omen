uniform mat4 ViewMatrix;
uniform mat4 Model;
uniform mat4 Projection;
uniform mat4 ModelViewProjection;
uniform mat3 NormalMatrix;
uniform vec4 Color;
uniform float Time;
uniform int InnerTessellationLevel1;
uniform int InnerTessellationLevel2;
uniform int OuterTessellationLevel1;
uniform int OuterTessellationLevel2;
uniform int OuterTessellationLevel3;
uniform int OuterTessellationLevel4;

const float PI = 3.14159265359;

#ifdef VERTEX_SHADER
in vec3 position;
in vec3 normal;
in vec2 texcoord;

//out vec3 vposition;
out float fog_factor;
out vec3 vnormal;
void main() {
    //vposition = position;
    vnormal = NormalMatrix*normal;
    fog_factor = 0.1;
    gl_Position = ModelViewProjection * Model * vec4(position,1);
}
#endif

#ifdef TESS_CONTROL_SHADE
layout(vertices = 3) out;

in  vec3 vposition[];
out vec3 tc_position[];

void main(void)
{
    gl_TessLevelInner[0] = InnerTessellationLevel1;
 	gl_TessLevelInner[1] = InnerTessellationLevel2;

 	gl_TessLevelOuter[0] = OuterTessellationLevel1;
 	gl_TessLevelOuter[1] = OuterTessellationLevel2;
 	gl_TessLevelOuter[2] = OuterTessellationLevel3;
 	gl_TessLevelOuter[3] = OuterTessellationLevel4;

    tc_position[gl_InvocationID] = vposition[gl_InvocationID];

 	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
#endif
#ifdef TESS_EVALUATION_SHADE
layout (triangles, equal_spacing, ccw) in;

in vec3 tc_position[];

//out vec3 te_position;
out vec3 vnormal;
out float fog_factor;
//out vec3 te_patchdistance;

void main()
{
    vec3 p0 = gl_TessCoord.x * tc_position[0];
    vec3 p1 = gl_TessCoord.y * tc_position[1];
    vec3 p2 = gl_TessCoord.z * tc_position[2];

    //te_patchdistance = gl_TessCoord;
    vec3 pos = (p0+p1+p2);

    float Time = Time *0.2;
    float PhaseX = 0;
    float PhaseZ = PI/3;
    float FrequencyX = 2.2*sin(Time);
    float FrequencyZ = 2.1*cos(Time*2);
    float Amplitude = 0.1;
    float anglex = (pos.x + PhaseX+Time) * FrequencyX;
    float anglez = (pos.z + PhaseZ+Time) * FrequencyZ;
    vec3 tx = vec3(0,cos(anglex)*Amplitude,0);
    vec3 nx = cross(tx,vec3(0,0,1));
    vec3 tz = vec3(0,-sin(anglez)*Amplitude,0);
    vec3 nz = cross(tz,vec3(1,0,0));
    vnormal = normalize(nx+nz);

    gl_Position = ModelViewProjection * vec4(pos,1);
    fog_factor = min(-gl_Position.z/100.0, 1.0);
}
#endif

#ifdef GEOMETRY_SHADE
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 te_position[3];
in vec3 te_patchdistance[3];

out vec3 vnormal;
out vec3 vpatchdistance;
out vec3 tri_distance;


void main()
{
    vec3 A = te_position[2] - te_position[1];
    vec3 B = te_position[1] - te_position[0];

    vnormal = normalize( cross(A, B) );

    vpatchdistance = te_patchdistance[0];
    tri_distance = vec3(1, 0, 0);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    vpatchdistance = te_patchdistance[1];
    tri_distance = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    vpatchdistance = te_patchdistance[2];
    tri_distance = vec3(0, 0, 1);
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}

#endif

#ifdef FRAGMENT_SHADER

in vec3 vnormal;
in float fog_factor;
//in vec3 te_position;
//in vec3 vpatchdistance;
//in vec3 tri_distance;

out vec4 color;
void main() {
    float l1 = length(vnormal);
    //float l2 = length(vpatchdistance);
    //float l3 = length(tri_distance);
    //float l4 = length(te_position);

    vec3 l = vec3(0.5,10,0.5);
    float br = 1.2*max(0.1,dot(l,vnormal/2));
    color = vec4(br*vec3(0.110,0.612,0.788),1); //Color;
    //color = vec4(vnormal.x, vnormal.y, vnormal.z, 1 );
    color = color * (1.0-fog_factor) + vec4(0.25, 0.75, 0.65, 1.0) * (fog_factor);
    color.a = 1;

}
#endif