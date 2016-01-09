/**
 * Uniforms
 */

// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec4 position;

void main() {
    gl_Position =  depthMVP * vec4(position,1);
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */

// Ouput data
layout(location = 0) out float fragmentdepth;

void main() {
    fragmentdepth = gl_FragCoord.z;
}

#endif
// END