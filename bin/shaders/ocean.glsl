#include "utils.glsl"

layout (binding=0) uniform sampler2D Texture;
layout (binding=1) uniform sampler2D Texture2;
uniform mat4 ModelViewProjection;
uniform vec3	  ViewPos;

#ifdef VERTEX_SHADER
layout(location=0) in vec3 vertex;
layout(location=1) in vec2 texcoord;

uniform int size = 32;
uniform float ocean_length;

out vec3 VPosition;
out vec3 FacetNormal;
out vec3 PatchDistance;
out vec3 TriDistance;
out vec2 TCoord;

void main()
{
	TCoord = vec2(gl_VertexID%2, (gl_VertexID/2)%2); //vec2(gl_InstanceID/size, 0);
	
	float l = 200.0+ocean_length*0.90;
	vec3 instanceOffset = vec3( (gl_InstanceID%3)*l, 0, (gl_InstanceID/3)*l);
	VPosition = vertex+instanceOffset;
	gl_Position = vec4(vertex+instanceOffset,1);

}
#endif

#include "ocean_tess.glsl"

#ifdef FRAGMENT_SHADER

////
// specularReflection returns the specular light reflection from given view- and light direction and normal
//

const float SpecularIntensity=52;
const float MaterialShininess=100;

vec3 specularReflection(vec3 ViewDir, vec3 Normal, vec3 Light)
{
  // Constant attenuation for directional light
  float attenuation = 1.0;
  vec4 lightSpecular = vec4(1.0,1.0,1.0,1.0);

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

void main()
{
	vec3 LightPos = vec3(12,1000,50);

	vec3 N = Normal;
	vec3 L = normalize(LightPos - Position);
    vec3 V = normalize(ViewPos - Position);    

	float d1 = abs(dot(Normal,vec3(1,0,0))+dot(Normal,vec3(0,0,1)))/2;

	vec3 normal = normalize(abs(texture(Texture2, TCoord))).xyz;
	float d = max(0,dot(Normal,normalize(vec3(12,1000,50))));
	color = d*vec4(0.43,1.0,1.0,1)+vec4(specularReflection(V,N,L),1)+d1*vec4(0.43,1.0,1.0,1)*0.5;
}
#endif