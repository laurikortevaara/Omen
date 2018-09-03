/**
 * Uniforms
 */
uniform sampler2D heightmap;
uniform sampler2D texturemap;

uniform mat4    ModelViewProjection;
uniform vec3	gEyeWorldPos;
uniform mat4    ViewMatrix;
uniform mat4    ViewProjMatrix;
uniform mat4    ModelMatrix;
uniform mat4    ModelViewMatrix;
uniform mat3    NormalMatrix;
uniform vec3	ViewPos;
uniform float	Shininess;
uniform vec3	LightDir;
uniform float	SpecularCoeff;
uniform vec4	MaterialDiffuse;
uniform vec4	MaterialSpecular;
uniform vec4	MaterialAmbient;
uniform bool    HasTexture;
uniform bool	RenderNormals = true;
uniform vec2	offsets[100];
uniform float	TessLevelInner;
uniform float	TessLevelOuter;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;

out VS_OUT
{
	vec3 tc_position;
    vec2 tc_texcoord;
	flat int	 texture_index;
} vs_out;

#define INDEX gl_InstanceID
#define GridWidth 1000.0f

void main() {
	vec4 pos = vec4(position,1);
	pos.x += (INDEX % 10)*GridWidth;
	pos.z += 100.0f-int(INDEX / 10)*GridWidth;
    gl_Position = pos;
	//gl_Position = ModelViewProjection * gl_Position;

	vs_out.tc_position = pos.xyz;
	vs_out.tc_texcoord = pos.xz * 0.0001;

	vs_out.texture_index = INDEX;
}
#endif

#ifdef TESS_CONTROL_SHADER

#define ID gl_InvocationID 
layout(vertices = 3) out;

in VS_OUT
{
    vec3 tc_position;
	vec2 tc_texcoord;
	flat int	 texture_index;
} tcs_in[];

out TCS_OUT
{
    vec3 tc_position;
	vec2 tc_texcoord;
	flat int	 texture_index;
} tcs_out[];

#define inner_tessellation_level 256
#define outer_tessellation_level 256

void main(void)
{
	if(gl_InvocationID==0)
	{
		gl_TessLevelInner[0] = inner_tessellation_level;
 		gl_TessLevelInner[1] = inner_tessellation_level;

 		gl_TessLevelOuter[0] = outer_tessellation_level;
 		gl_TessLevelOuter[1] = outer_tessellation_level;
 		gl_TessLevelOuter[2] = outer_tessellation_level;
 		gl_TessLevelOuter[3] = outer_tessellation_level;
	}

    tcs_out[ID].tc_position =  tcs_in[ID].tc_position;	
	tcs_out[ID].tc_texcoord =  tcs_in[ID].tc_texcoord;	
	tcs_out[ID].texture_index = tcs_in[ID].texture_index;
 	gl_out[ID].gl_Position = gl_in[ID].gl_Position;
}
#endif

#ifdef TESS_EVALUATION_SHADER
layout (triangles, equal_spacing, ccw) in;

in TCS_OUT
{
    vec3 tc_position;
	vec2 tc_texcoord;
	flat int	 texture_index;
} tes_in[];

out TES_OUT
{
    vec3 tc_position;
	vec2 tc_texcoord;
	flat int	 texture_index;
	float h;
} tes_out;


void main()
{
    vec4 p0 = gl_TessCoord.x * gl_in[0].gl_Position;
    vec4 p1 = gl_TessCoord.y * gl_in[1].gl_Position;
    vec4 p2 = gl_TessCoord.z * gl_in[2].gl_Position;

	vec2 t0 = gl_TessCoord.x * tes_in[0].tc_texcoord;
    vec2 t1 = gl_TessCoord.y * tes_in[1].tc_texcoord;
    vec2 t2 = gl_TessCoord.z * tes_in[2].tc_texcoord;

	tes_out.tc_texcoord = (t0+t1+t2);
	tes_out.texture_index = tes_in[0].texture_index;

	vec4 pos = (p0+p1+p2);

	float h = length(texture(heightmap,tes_out.tc_texcoord).rgb);
	tes_out.h = h;
	pos.y += h*500.0f;
	
	gl_Position = ModelViewProjection * pos;
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
in TES_OUT
{
    vec3 tc_position;
	vec2 tc_texcoord;
	flat int	 texture_index;
	float h;
} fs_in;
  
out vec4 out_color;

void main() {
	out_color = vec4(1);
	
	vec2 tc_texcoord = fs_in.tc_texcoord;
	out_color = texture(texturemap,tc_texcoord);
}
#endif
// END
