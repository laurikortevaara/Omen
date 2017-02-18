/**
 * Uniforms
 */
uniform sampler2D Texture;
uniform mat4    Model;
uniform bool    Enabled;
uniform int		BorderLeft;
uniform int		BorderRight;
uniform int		BorderTop;
uniform int		BorderBottom;
uniform int		ViewWidth;
uniform int		ViewHeight;
uniform int		SpriteWidth;
uniform int		SpriteHeight;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec4 position;
layout(location=1) in vec2 texcoord;

out Data{
    vec4 position;
	vec4 posScreen;
    vec2 texcoord;
} dataOut;

void main() {
    dataOut.position = position;
    dataOut.texcoord = texcoord;
	dataOut.posScreen = (Model*position)/position.w;

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
	vec4 posScreen;
    vec2 texcoord;
} dataIn;

out vec4 out_color;

void main() {
    vec4 vpos = dataIn.position;
	vec4 posScreen = dataIn.posScreen;
    vec2 tcoord = dataIn.texcoord;

	int x = int(tcoord.x * ViewWidth)+1;
	int y = int(tcoord.y * ViewHeight)+1;
	float tx = 0.0;
	float ty = 0.0;

	if(x <= BorderLeft)
		tx = float(x)/SpriteWidth;
	else
	if(x >= (ViewWidth-BorderRight))
		tx = float((ViewWidth+1)-x)/SpriteWidth;
	else
		tx = 0.5;

	if(y <= BorderTop)
		ty = float(y)/SpriteHeight;
	else
	if(y >= (ViewHeight-BorderBottom))
		ty = float((ViewHeight+1)-y)/SpriteHeight;
	else
		ty = 0.5;

	out_color = texture(Texture, vec2(tx,ty));
}


#endif
// END