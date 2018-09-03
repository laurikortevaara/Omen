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
    vec3 normal;
} dataOut;

void main() {
    dataOut.normal = normal;
    dataOut.position = position;
    gl_Position = ModelViewProjection * Model * position;
}
#endif

#ifdef GEOMETRY_SHADER
layout(triangles) in;
layout(line_strip, max_vertices = 100) out;
in Data{
    vec4 position;
    vec3 normal;
} dataIn[];
void main() {

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position+ModelView*vec4(dataIn[0].normal,1)*10;
    EmitVertex();


    EndPrimitive();
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