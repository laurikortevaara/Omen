/**
 * Uniforms
 */
uniform sampler2D Texture;
uniform samplerCube uTexEnv;
uniform mat4 invPV;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER

const vec2 madd=vec2(0.5,0.5);

layout(location=0) in vec2 vertexIn;

out vec3 dir;

void main() {
    vec2 pos  = vec2( (gl_VertexID & 2)>>1, 1 - (gl_VertexID & 1)) * 2.0 - 1.0;
    vec4 front= invPV * vec4(pos, -1.0, 1.0);
    vec4 back = invPV * vec4(pos,  1.0, 1.0);

    dir=back.xyz / back.w - front.xyz / front.w;
    gl_Position = vec4(pos,-1.0,1.0);
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
in vec3 dir;
out vec4 fragColor;
void main() 
{
	fragColor = texture(uTexEnv, dir);
}
#endif
// END