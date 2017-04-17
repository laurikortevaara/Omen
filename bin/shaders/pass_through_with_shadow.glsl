/**
* Uniforms
*/



uniform sampler2D TextureMap;
uniform sampler2D shadowMap;

uniform mat4      ModelViewProjection;
uniform mat3	  ModelView3x3;
uniform mat4	  InverseViewProjection;
uniform mat4	  ModelMatrix;
uniform mat4	  ViewMatrix;
uniform mat4	  DepthBiasMVP;

uniform vec3	  LightPos;

uniform bool	  HasTexture = true;
uniform float     DepthBias = 0.01;

uniform vec4	  MaterialDiffuse;
uniform vec4	  MaterialEmissive;
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
    vec4 ShadowCoord;
    vec2 uv;
	vec3 normal;
} dataOut;

void main()
{
    gl_Position =  ModelViewProjection * vec4(position,1);
    dataOut.ShadowCoord = DepthBiasMVP * vec4(position,1);
    dataOut.uv = uv;
	dataOut.normal = normal;
}
#endif

#ifdef FRAGMENT_SHADER
/**
* Fragment Shader
*
*/

in Data {
	vec4 ShadowCoord;
	vec2 uv;
	vec3 normal;
} dataIn;

out vec4 out_color;

void main() {
    vec4 ShadowCoord = dataIn.ShadowCoord;
    vec2 uv			 = dataIn.uv;

	// Shadow depth test
	float visibility = 1.0;
    float bias = 0.01;
	float compare = (ShadowCoord.z-bias);
	float fDepth = texture( shadowMap, ShadowCoord.xy/ShadowCoord.w ).z;
    if (  fDepth < compare  )
        visibility = 0.5;

	out_color = HasTexture ? texture(TextureMap, uv) : MaterialDiffuse;
	out_color = vec4(abs(dataIn.normal),1)*visibility;
	out_color.a = 1;
}


#endif
// END
