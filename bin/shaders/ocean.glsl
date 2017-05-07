#include "utils.glsl"

uniform sampler2D Texture;
//uniform sampler2D Texture2;
uniform mat4 ModelViewProjection;

#ifdef VERTEX_SHADER
layout(location=0) in vec3 vertex;
layout(location=1) in vec2 texcoord;

uniform int size = 32;

out vec2 tcoord;

void main()
{
	tcoord = texcoord;
	tcoord = (texcoord+vec2(gl_InstanceID%size, gl_InstanceID/size))/size;
	
	float x = texture(Texture,tcoord).r;
	float y = texture(Texture,tcoord).g;
	float z = texture(Texture,tcoord).b;
	gl_Position = ModelViewProjection*(vec4(vertex,1)+vec4(vec3((gl_InstanceID%size)*2+x,y,2*(gl_InstanceID/size)+z),1));
}
#endif

#ifdef FRAGMENT_SHADER
out vec4 color;
in vec2 tcoord;
void main()
{
	//color = 8*texture(Texture,tcoord)+2*rand(gl_FragCoord.xy*iGlobalTime)*abs(sin(iGlobalTime*0.01));
	//color /= 10;
	color = vec4(tcoord,0,1); // texture(Texture,tcoord);
}
#endif