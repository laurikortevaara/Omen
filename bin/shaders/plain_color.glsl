/**
 * Uniforms
 */
uniform vec4	  Color;
uniform mat4      Model;
uniform bool      Enabled;
uniform bool      Hovered;
uniform bool      Pressed;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec4 position;

out Data{
    vec4 position;
} dataOut;

void main() {
    dataOut.position = position;
    gl_Position = Model*(position);
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
in Data {
    vec4 position;
} dataIn;

out vec4 out_color;

void main() {
    vec4 vpos = dataIn.position;
	out_color = Color;
}

#endif
// END
