/**
 * Uniforms
 */
uniform mat4 ModelViewProjection;

const int MY_ARRAY_SIZE = 512;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER

const vec2 madd=vec2(0.5,0.5);

layout(location=0) in vec3 vertexIn;
out vec3 p;
void main() {
    p = vertexIn;
    gl_Position = ModelViewProjection*vec4((vertexIn-vec3(madd,0))*2,1.0);
}
#endif

#ifdef FRAGMENT_SHADER

/**
 * Fragment Shader
 *
 */
 layout (std430, binding = 3) buffer ShaderSSBO
 {
     float fps_value[512];
 };

in vec3 p;
out vec4 fragColor;
void main()
{
    float f = fps_value[min(512,int(abs(p.x*512.0)))];
    f = smoothstep(p.y*512.0-0.01, p.y*512.0+0.01, f );
    fragColor = vec4(f, 0, 0, f);
}
#endif
// END
