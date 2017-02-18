/**
 * Uniforms
 */
uniform sampler2D tex;
uniform sampler2D normalmap;
uniform mat4      ModelViewProjection;
uniform mat4      Model;
uniform vec3	  ViewPos;
uniform float	  Shininess;
uniform vec3	  LightDir;
uniform float	SpecularCoeff;

**/
/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec3 normal;
layout(location=3) in vec3 tangent;

out Data{
	vec3 fragPos;
    vec3 position;
    vec2 texcoord;
	vec3 normal;
	vec3 tangent;
	vec3 normalCameraSpace;
	float distToCamera;
} dataOut;

void main() {
    dataOut.fragPos = vec3( Model * vec4(position,1) );
	dataOut.position = position;
    dataOut.texcoord = texcoord;
    dataOut.normal = normal;
	dataOut.tangent = tangent;
	dataOut.normalCameraSpace = vec3( Model * vec4(normal, 1) );

	vec4 cs_position = ModelViewProjection * Model * vec4(position,1);
    //= -cs_position.z;

	gl_Position = ModelViewProjection * Model * vec4(position,1);
	 dataOut.distToCamera = gl_Position.w;
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
in Data {
	vec3 fragPos;
    vec3 position;
    vec2 texcoord;
    vec3 normal;
	vec3 tangent;
	vec3 normalCameraSpace;
	float distToCamera;
} dataIn;

out vec4 out_color;

vec3 CalcBumpedNormal(vec3 n, vec3 t, vec2 TexCoord0)
{
    vec3 Normal = normalize(n);
    vec3 Tangent = normalize(t);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(normalmap, TexCoord0).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);
    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

void main() {
	vec3 fragPos = dataIn.fragPos;
    vec3 vpos = dataIn.position;
    vec2 tcoord = dataIn.texcoord;
    vec3 normal = -normalize(dataIn.normal);
	vec3 n = normalize(vec3( Model * vec4(-dataIn.normal, 1) ));

	vec3 t = normalize(vec3(Mode*vec4(dataIn.tangent,1)));
	
	vec3 n = CalcBumpedNormal(n,t,tcoord);

	vec3 lpos = vec3(100,100,0);
	vec3 l = normalize(lpos-fragPos);

	float ldot = max(dot(n,l),0.0);

	out_color = vec4(vec3(ldot),1);
    
	
}


#endif
// END