
/**
 * Uniforms
 */
uniform mat4 Projection;
uniform sampler2D Texture;
uniform sampler2D Text;
uniform vec4 TextColor;
uniform mat4 Model;

uniform vec4      FontColor;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

void main()
{
    gl_Position = Projection*Model*vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
smooth in vec2 TexCoords;

out vec4 out_color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(Text, TexCoords).r * 0.5);
    out_color = TextColor * sampled;
}

#endif
// END
