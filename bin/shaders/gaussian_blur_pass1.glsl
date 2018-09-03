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

vec4 mainImage( sampler2D iChannel0, vec2 fragCoord )
{
    if (fragCoord.x < iMouse.x || Blur == 0) {
    	return texture(iChannel0, fragCoord / iResolution.xy);
    }
    
	int iBlur = int(variableBlur(Blur));
    int num_weights = (KERNEL_SIZE+iBlur + 1) / 2;
	float w0 = gaussian(0.0);
	float sum = 0.0;
        
    vec3 col = texture(iChannel0, fragCoord / iResolution.xy).rgb * w0;
    for (int i = 1; i < num_weights; ++i) {
		float w = gaussian(float(i));
		sum += 2*w;
    	col += texture(iChannel0, (fragCoord - vec2(i, 0)) / iResolution.xy).rgb * w;
        col += texture(iChannel0, (fragCoord + vec2(i, 0)) / iResolution.xy).rgb * w;
    }
	sum -= w0;
    
    return vec4( col / sum,1 );
}

void main() 
{
    fragColor = mainImage(Texture, gl_FragCoord.xy);
}
#endif
// END