/**
 * Uniforms
 */
uniform mat4      ModelViewProjection;
uniform mat4      Model;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec4 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec4 normal;

out Data{
    vec4 position;
    vec2 texcoord;
    flat vec4 normal;
} dataOut;

void main() {
    dataOut.position = position;
    dataOut.texcoord = texcoord;
    dataOut.normal = normal;

    gl_Position = ModelViewProjection * Model * position;
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
in Data {
    vec4 position;
        vec2 texcoord;
        flat vec4 normal;
} dataIn;

out vec4 out_color;

void main() {
    vec4 vpos = dataIn.position;
    vec2 tcoord = dataIn.texcoord;
    vec4 normal = dataIn.normal;

    out_color = vec4(normal.x, normal.y, normal.z, 1);
}


#endif
// END