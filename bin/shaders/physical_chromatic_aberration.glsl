#include "utils.glsl"

/**
 * Uniforms
 */
uniform sampler2D Texture;
uniform float Blur;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER

const vec2 madd=vec2(0.5,0.5);

layout(location=0) in vec2 vertexIn;

out vec2 textureCoord;

void main() {
   textureCoord = -1.0*vertexIn.xy*madd+madd; // scale vertex attribute to [0-1] range
   gl_Position = vec4(vertexIn.xy,0.0,1.0);
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
in vec2 textureCoord;

out vec4 fragColor;

#define SIGMA 11.16
#define KERNEL_SIZE 67

float gaussian(in float x) {
	return exp(-( pow(x, 2.0) / (pow(SIGMA+variableBlur(Blur), 2.0) * 2.0)));   
}

vec4 texture(sampler2D iChannel0, vec2 uv) {
	return vec4(texture2D(iChannel0,vec2(uv.x,uv.y)));
}

vec4 mainImage( sampler2D iChannel0, in vec2 fragCoord )
{
    vec3 refractiveIndex = vec3(1.0, 1.015, 1.03);
    vec2 uv = fragCoord.xy / iResolution.xy;
    
	vec2 normalizedTexCoord = vec2(2.0, 2.0) * uv - vec2(1.0, 1.0);    // [0, 1] -> [-1, 1]
    vec3 texVec = vec3(normalizedTexCoord, 1.0);
    vec3 normalVec = vec3(0.0, 0.0, -1.0);
    
	vec3 redRefractionVec   = refract(texVec, normalVec, refractiveIndex.r);
    vec3 greenRefractionVec = refract(texVec, normalVec, refractiveIndex.g);
    vec3 blueRefractionVec  = refract(texVec, normalVec, refractiveIndex.b);
    
	vec2 redTexCoord   = ((redRefractionVec / redRefractionVec.z).xy     + vec2(1.0, 1.0)) / vec2(2.0, 2.0);
    vec2 greenTexCoord = ((greenRefractionVec / greenRefractionVec.z).xy + vec2(1.0, 1.0)) / vec2(2.0, 2.0);
    vec2 blueTexCoord  = ((blueRefractionVec / blueRefractionVec.z).xy   + vec2(1.0, 1.0)) / vec2(2.0, 2.0);
    
    return vec4
    (
        texture2D(iChannel0, redTexCoord).r, 
        texture2D(iChannel0, greenTexCoord).g, 
        texture2D(iChannel0, blueTexCoord).b, 
        1.0
    );
}

vec4 mainImage2( sampler2D iChannel0, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;

	float amount = 0.0;
	
	amount = (1.0 + sin(iGlobalTime*6.0)) * 0.5;
	amount *= 1.0 + sin(iGlobalTime*16.0) * 0.5;
	amount *= 1.0 + sin(iGlobalTime*19.0) * 0.5;
	amount *= 1.0 + sin(iGlobalTime*27.0) * 0.5;
	amount = pow(amount, 3.0);

	amount *= 0.05;
	
    vec3 col;
    col.r = texture2D( iChannel0, vec2(uv.x+amount,uv.y) ).r;
    col.g = texture2D( iChannel0, uv ).g;
    col.b = texture2D( iChannel0, vec2(uv.x-amount,uv.y) ).b;

	col *= (1.0 - amount * 0.5);
	
    return vec4(col,1.0);
}

void main() 
{
    fragColor = mix(mainImage(Texture, gl_FragCoord.xy), mainImage2(Texture, gl_FragCoord.xy),0.1);
}
#endif
// END