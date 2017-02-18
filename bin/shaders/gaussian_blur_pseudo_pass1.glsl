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
	return vec4(texture2D(iChannel0,vec2(uv.x,1.0-uv.y)));
}

vec4 BlurV (sampler2D source, vec2 size, vec2 uv, float radius) {

	if (radius >= 1.0)
	{
		vec4 A = vec4(0.0); 
		vec4 C = vec4(0.0); 

		float height = 1.0 / size.y;

		float divisor = 0.0; 
        float weight = 0.0;
        
        float radiusMultiplier = 1.0 / radius;

        for (float y = -20.0; y <= 20.0; y++)
		{
			A = texture(source, uv + vec2(0.0, y * height));
            	
            	weight = SCurve(1.0 - (abs(y) * radiusMultiplier)); 
            
            	C += A * weight; 
            
			divisor += weight; 
		}

		return vec4(C.r / divisor, C.g / divisor, C.b / divisor, 1.0);
	}

	return texture(source, uv);
}



vec4 mainImage( sampler2D iChannel0, in vec2 fragCoord )
{    
    vec2 uv = fragCoord.xy / iResolution.xy; 
    
    // Apply vertical blur to buffer A
	return BlurV(iChannel0, iResolution.xy, uv, 20.0);
}

void main() 
{
    fragColor = mainImage(Texture, gl_FragCoord.xy);
	//fragColor = texture(Texture, textureCoord);
}
#endif
// END