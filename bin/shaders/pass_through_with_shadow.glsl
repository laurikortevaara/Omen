/**
* Uniforms
*/



uniform sampler2D TextureMap;
uniform sampler2D shadowMap;

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
uniform float     MaterialShininess;
uniform float     AmbientFactor;

uniform float     LightIntensity;

#include "utils.glsl"

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
	vec4 pos;
    vec4 ShadowCoord;
    vec2 uv;
	vec3 normal;
} dataOut;

void main()
{
    gl_Position =  ModelViewProjection * vec4(position,1);
    dataOut.ShadowCoord = DepthBiasMVP * vec4(position,1);
    dataOut.uv = uv;
	dataOut.normal = mat3(ModelMatrix)*normal; //mat3(transpose(inverse(ModelMatrix)))*normal;
	dataOut.pos = ModelMatrix*vec4(position,1);
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
vec3 diffuseReflection(vec3 Normal, vec3 Light)
{
	return vec3(max(dot(Normal,Light),0));
}

////
// specularReflection returns the specular light reflection from given view- and light direction and normal
//
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
	float specularStrength=10.0;
	vec3 reflectDir = reflect(-Light, Normal);  
	float spec = pow(max(dot(ViewDir, reflectDir), 0.0), 128);
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
	vec4 ShadowCoord;
	vec2 uv;
	vec3 normal;
} dataIn;

out vec4 out_color;

void main() {
    vec4 ShadowCoord = dataIn.ShadowCoord;
    vec2 uv			 = dataIn.uv;
	vec3 N			 = normalize(dataIn.normal);

	// Calculate the Light vector
	vec3 L = lightVector(dataIn.pos.xyz,LightPos);

	// Calculate the view direction, eye to fragment vector
	vec3 V = viewDirection(dataIn.pos.xyz);

	// Shadow depth test
	float visibility = 1.0;
    float bias = 0.005;
	float compare = (ShadowCoord.z-bias);
	visibility = max(0.15,PCF3(shadowMap, vec2(8192.0), ShadowCoord.xy, compare,1)); //texture( shadowMap, ShadowCoord.xy/ShadowCoord.w ).z);
   /* float fDepth = texture( shadowMap, ShadowCoord.xy/ShadowCoord.w ).z;
    if (  fDepth < compare  )
        visibility = 0.5;
	*/

	// texture or albedo color
	out_color = HasTexture ? texture(TextureMap, uv) : MaterialDiffuse;

	// Final fragment color
	float a = out_color.a;
	//out_color *= visibility * (diffuseReflection(N,L)+specularReflection(V,N,L)) *;
	out_color = visibility*vec4(diffuseReflection(N,L)+specularReflection(V,N,L),1);
	out_color.a = a;
}


#endif
// END
