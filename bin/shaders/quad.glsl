uniform sampler2D Texture;

/**
 * Uniforms
 */

// Values that stay constant for the whole mesh.

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec2 position;

out vec2 uv;

void main() {
    gl_Position =  vec4(position,0,1);
	uv = 0.5*(position+vec2(1,1));
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */

// Ouput data
in vec2 uv;
layout(location = 0) out vec4 color_out;

void main() {
	float depthValue = texture(Texture, uv).r;
    color_out = vec4(vec3(depthValue), 1.0);
}

#endif
// END