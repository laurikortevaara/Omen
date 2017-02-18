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
layout(location=0) in vec3 position;

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
out float fragmentdepth;
//out vec3 color;

void main() {
    fragmentdepth = gl_FragCoord.z;
	//color = vec3(1,0,0);
}

#endif
// END
