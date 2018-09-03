/**
* Uniforms
*/


#include "utils.glsl"

layout(location = 0) uniform sampler2D DiffuseColorMap;
layout(location = 1) uniform sampler2D NormalMap;
layout(location = 2) uniform sampler2D SpecularColorMap;
layout(location = 3) uniform sampler2D shadowMap;
layout(location = 4) uniform samplerCube envMap;

uniform bool      HasDiffuseTexture;
uniform bool	  HasNormalTexture;
uniform bool      HasSpecularColorTexture;


uniform mat4      ModelViewProjection;
uniform mat3	  ModelView3x3;
uniform mat4	  InverseView;
uniform mat4	  InverseViewProjection;
uniform mat4	  ModelMatrix;
uniform mat4	  ViewMatrix;
uniform mat4	  DepthBiasMVP;

uniform vec3	  LightPos;
uniform vec3	  ViewPos;

uniform bool	  HasTexture = true;
uniform float     DepthBias = 0.01;

uniform vec4	  MaterialAmbient;
uniform vec4	  MaterialDiffuse;
uniform vec4	  MaterialEmissive;
uniform vec4	  MaterialSpecular;

uniform float	  DiffuseIntensity;
uniform float	  SpecularIntensity;
uniform float     MaterialShininess;
uniform float     AmbientFactor;

uniform float     LightIntensity;


/**
* Vertex Shader
*
*/
#ifdef VERTEX_SHADER
layout(location=0) in vec3 position;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 normal;
layout(location=3) in vec3 tangent;
layout(location=4) in vec3 bitangent;

out Data
{
	vec4 pos;			// Position in world space
	vec4 pos_eye;		// Position in eye/view space
    vec4 ShadowCoord;
    vec2 uv;
	vec3 normal;		// Normal in world space
	vec3 normal_eye;	// Normal in eye/view space
	mat3 TBN;
} dataOut;

void main()
{
    gl_Position =  ModelViewProjection * vec4(position,1);
    dataOut.ShadowCoord = DepthBiasMVP * vec4(position,1);
    dataOut.uv = uv;
	dataOut.normal = mat3(ModelMatrix)*normal; //mat3(transpose(inverse(ModelMatrix)))*normal;
	dataOut.normal_eye = vec3(ViewMatrix*ModelMatrix*vec4(normal,0));
	dataOut.pos = ModelMatrix*vec4(position,1);
	dataOut.pos_eye = ViewMatrix*ModelMatrix*vec4(position,1);

	vec3 T = normalize(vec3(ModelMatrix * vec4(tangent,   0.0)));
	vec3 N = normal; //normalize(vec3(ModelMatrix * vec4(normal,    0.0)));
	vec3 B = normalize(cross(T,N));

	mat3 TBN = transpose(mat3(T, B, N));
	dataOut.TBN = TBN;
}
#endif

#ifdef FRAGMENT_SHADER
/**
* Fragment Shader
*
*/


////
// diffuseReflection returns the diffuse light reflection from given light direction and normal
//
float diffuseReflection(vec3 Normal, vec3 Light)
{
	return max(dot(Normal,Light),0);
}

////
// specularReflection returns the specular light reflection from given view- and light direction and normal
//
vec3 specularReflection(vec3 ViewDir, vec3 Normal, vec3 Light, vec2 uv)
{
  // Constant attenuation for directional light
  float attenuation = 1.0;
  vec4 lightSpecular = HasSpecularColorTexture?texture(SpecularColorMap, uv):vec4(1.0,1.0,1.0,1.0);

  vec3 specularReflection;
  if (dot(Normal, Light) < 0.0) // light source on the wrong side?
  {
	specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
  }
  else
  {
	float specularStrength=SpecularIntensity;
	vec3 reflectDir = reflect(-Light, Normal);  
	float spec = pow(max(dot(ViewDir, reflectDir), 0.0), MaterialShininess);
	vec4 specular = specularStrength * spec * lightSpecular;  
    specularReflection = specular.rgb; //attenuation * vec3(lightSpecular) * vec3(MaterialSpecular) * pow(max(0.0, dot(reflect(-Light, Normal), ViewDir)), 50.0); //MaterialShininess);
  }
  //specularReflection = clamp(specularReflection, vec3(0), vec3(1));
  return specularReflection;
}

////
// lightVector returns the vector from light to given fragment
//
vec3 lightVector(vec3 fragmentPos, vec3 lightPos)
{
	return normalize(lightPos-fragmentPos);
}


////
// viewDirection returns the direction vector from fragment to eye
//
vec3 viewDirection(vec3 fragmentPos)
{
	return normalize(ViewPos-fragmentPos);
}


in Data {
	vec4 pos;
	vec4 pos_eye;
	vec4 ShadowCoord;
	vec2 uv;
	vec3 normal;
	vec3 normal_eye;
	mat3 TBN;
} dataIn;

out vec4 out_color;



void main() {
	vec2 uv = dataIn.uv;
	vec3 N = dataIn.normal; 
	vec3 normal = normalize(dataIn.normal_eye);
	
	vec3 vpos = ViewPos;
	vpos.x *= -1;
	vpos.z *= -1;

	vec3 I = normalize(dataIn.pos_eye.xyz);
    vec3 R = reflect(I, normal);
	R  = vec3(inverse(ViewMatrix) * vec4(R, 0.0));

	if(HasNormalTexture)
	{
		N = texture(NormalMap, uv).rgb;
		N = normalize(N * 2.0 - 1.0);   
		N = normalize(dataIn.TBN * N);
	}
   
    vec3 L = normalize(LightPos - dataIn.pos.xyz);
    vec3 V = normalize(ViewPos - dataIn.pos.xyz);    
	
	float d = diffuseReflection(N,L);
	vec4 c = HasDiffuseTexture?texture(DiffuseColorMap,uv):MaterialDiffuse;
	out_color = d*c + vec4(specularReflection(V,N,L, dataIn.uv),1);
	out_color = c;
		
	out_color = 7*texture(DiffuseColorMap,uv) + 3*texture(envMap, R);
	out_color /= 10;

	//out_color = texture(envMap, R);
	//out_color = texture(DiffuseColorMap,uv);
}

#endif
// END
