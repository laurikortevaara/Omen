#include "utils.glsl"

layout (binding=0) uniform sampler2D Texture;
layout (binding=1) uniform sampler2D Texture2;
uniform mat4 ModelViewProjection;

#ifdef VERTEX_SHADER
layout(location=0) in vec3 vertex;
layout(location=1) in vec2 texcoord;

uniform int size = 32;

out vec2 tcoord;
out vec4 normal;

void main()
{
	tcoord = texcoord;
	tcoord = (texcoord+vec2(gl_InstanceID%size, gl_InstanceID/size))/size;
	
	float x = texture(Texture,tcoord).r;
	float y = texture(Texture,tcoord).g;
	float z = texture(Texture,tcoord).b;

	normal = normalize(texture(Texture2,tcoord));
	gl_Position = ModelViewProjection*(vec4(vertex,1)+vec4(-gl_InstanceID/size,0,gl_InstanceID%size,0)*2 + vec4(x*0.001,y,z*0.001,0));
	//gl_Position = ModelViewProjection*vec4(vertex,1); //vec4(x,y,z,1);
}
#endif

#ifdef FRAGMENT_SHADER
out vec4 color;
in vec2 tcoord;
in vec4 normal;
void main()
{
	//color = 8*texture(Texture,tcoord)+2*rand(gl_FragCoord.xy*iGlobalTime)*abs(sin(iGlobalTime*0.01));
	//color /= 10;
	color = vec4(tcoord,0,1); //
	float d = dot(normalize(normal),normalize(vec4(0.03,-1,0,1)));
	//color = texture(Texture2,tcoord);
	color = vec4(d,d,d,1);
	//color = normal;
	//color /= 2;
}
#endif