#include "utils.glsl"

layout (binding=0) uniform sampler2D Texture;
layout (binding=1) uniform sampler2D Texture2;
uniform mat4 ModelViewProjection;

#ifdef VERTEX_SHADER
layout(location=0) in vec3 vertex;
layout(location=1) in vec2 texcoord;

uniform int size = 32;

out vec3 VPosition;
out vec3 FacetNormal;
out vec3 PatchDistance;
out vec3 TriDistance;
out vec2 TCoord;

void main()
{
	TCoord = vec2(gl_VertexID%2, (gl_VertexID/2)%2); //vec2(gl_InstanceID/size, 0);
	VPosition = vertex;

	gl_Position = vec4(vertex,1);

}
#endif

#include "ocean_tess.glsl"

#ifdef FRAGMENT_SHADER
out vec4 color;

in vec3 FacetNormal;
in vec3 PatchDistance;
in vec3 TriDistance;
in vec3 Normal;
in vec2 TCoord;

void main()
{
	vec3 normal = normalize(abs(texture(Texture2, TCoord))).xyz;
	float d = max(0,dot(Normal,normalize(vec3(12,1000,50))));
	color = d*vec4(0.43,1.0,1.0,1);
}
#endif