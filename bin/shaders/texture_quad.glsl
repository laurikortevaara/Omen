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

void main() 
{
	vec2 fragCoord = gl_FragCoord.xy;
	vec2 uv = (fragCoord.xy / iResolution.xy);
    fragColor = texture(Texture, textureCoord);
}
#endif
// END