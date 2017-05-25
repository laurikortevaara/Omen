uniform mat4 ViewMatrix;
uniform float viewport;
uniform mat4 inv_proj;
uniform mat3 inv_view_rot;

uniform samplerCube DiffuseColorMap;  
//uniform sampler2D DiffuseColorMap;  


#ifdef VERTEX_SHADER
in vec3 position;

out vec3 vposition;
out vec3 TexCoords;

void main() {
    vposition = position;

    gl_Position = ViewMatrix*vec4(position,1);
	TexCoords = position;
}
#endif

#ifdef FRAGMENT_SHADER
in vec3 vposition;
in vec3 TexCoords;
out vec4 color;
void main() 
{
     color = texture(DiffuseColorMap, TexCoords);
	 //color += vec4(TexCoords,1);
}
#endif
