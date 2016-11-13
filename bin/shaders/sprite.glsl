/**
 * Uniforms
 */
uniform sampler2D Texture;
uniform mat4      Model;
uniform bool      Enabled;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec4 position;
layout(location=1) in vec2 texcoord;

out Data{
    vec4 position;
    vec2 texcoord;
} dataOut;

void main() {
    dataOut.position = position;
    dataOut.texcoord = texcoord;

    gl_Position = Model*position;
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
} dataIn;

out vec4 out_color;

void main() {
    vec4 vpos = dataIn.position;
    vec2 tcoord = dataIn.texcoord;

    out_color = texture(Texture, tcoord);
    if(!Enabled)
        out_color = out_color*vec4(1.0);	
}


#endif
// END