/**
 * Uniforms
 */
uniform mat4      Model;
uniform mat4      ModelViewProjection;
uniform mat4      ModelView;
uniform mat4      ModelViewInverse;
uniform mat4      NormalMatrix;


/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec4 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec3 normal;

out Data{
    vec4 position;
} dataOut;

void main() {
    gl_Position = ModelViewProjection * Model * position;
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
out vec4 out_color;

void main() {
    out_color = vec4(1);
}


#endif
// END