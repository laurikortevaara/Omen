#include "utils.glsl"
#include "pnoise2.glsl"

layout (binding=0) uniform sampler2D Texture;
layout (binding=1) uniform sampler2D Texture2;
layout (binding=2) uniform sampler2D Texture3;
layout (binding=4) uniform samplerCube envmap;

uniform mat4	ModelViewProjection;
uniform vec3	ViewPos;
uniform mat4	ViewMatrix;
uniform mat4	ModelMatrix;
uniform bool    drawNormals= false;
uniform float   PerlinSize;
uniform float   PerlinScale;

#ifdef VERTEX_SHADER
layout(location=0) in vec3 vertex;
layout(location=1) in vec2 texcoord;

uniform int size = 32;
uniform float ocean_length;
uniform int PatchCount;
uniform float	PatchSize;

out vec3 VPosition;
out vec3 FacetNormal;
out vec3 PatchDistance;
out vec3 TriDistance;
out vec2 TCoord;
vec4 pos_eye;		// Position in eye/view space
vec3 normal_eye;

void main()
{
    //float l = PatchSize+ocean_length*0.90;
	//float pc = PatchCount;
	float x = gl_InstanceID % PatchCount;
	float z = gl_InstanceID / PatchCount;
	vec3 instanceOffset = vec3(x,0.0,z)*100.0f;
	VPosition = vertex+instanceOffset;
	gl_Position = ModelViewProjection*vec4(vertex+instanceOffset,1);

	TCoord = vec4(vertex+instanceOffset,1).xz/(PatchCount*PatchSize);

	pos_eye = vec4(0);
	normal_eye = vec3(0);
}
#endif

#include "ocean_tess.glsl"

#ifdef FRAGMENT_SHADER

////
// specularReflection returns the specular light reflection from given view- and light direction and normal
//

const float SpecularIntensity=10;
const float MaterialShininess=200;

vec3 specularReflection(vec3 ViewDir, vec3 Normal, vec3 Light)
{
  // Constant attenuation for directional light
  float attenuation = 1.0;
  vec4 lightSpecular = vec4(1.0,0.8,0.2,1.0);

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


out vec4 color;

in vec3 FacetNormal;
in vec3 PatchDistance;
in vec3 TriDistance;
in vec3 Normal;
in vec2 TCoord;
in vec3 Position;
in vec4 pos_eye;
in vec3 normal_eye;

void main()
{
	vec3 LightPos = vec3(12,1000,50);

	vec3 N = Normal;
	vec3 L = normalize(LightPos - Position);
    vec3 V = normalize(ViewPos - Position);    

	float d1 = abs(dot(Normal,vec3(1,0,0))+dot(Normal,vec3(0,0,1)))/2;

	vec3 normal = normalize(abs(texture(Texture2, TCoord))).xyz;
	float d = max(0,dot(Normal,normalize(vec3(12,1000,50))));
	//float d2 = clamp( 1.0-max(0,dot(Normal, vec3(0,1,0) ) ));
	color = /*d*vec4(0.43,1.0,1.0,1)+*/vec4(specularReflection(V,N,L),1); //+d1*vec4(0.43,1.0,1.0,1)*0.5;

	vec3 I = normalize(pos_eye.xyz);
    vec3 R = reflect(I, normal);
	R  = vec3(inverse(ViewMatrix) * vec4(R, 0.0));

	color = vec4(specularReflection(V,N,L),1)+0.05*texture(envmap, R);
	if(drawNormals)
	  color = vec4(N,1);

	color = texture(Texture3, TCoord);


}
#endif