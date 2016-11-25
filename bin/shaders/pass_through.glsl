/**
 * Uniforms
 */
uniform sampler2D tex;
uniform mat4      ModelViewProjection;
uniform mat4      Model;
uniform vec3	  ViewPos;
uniform float	  Shininess;
uniform vec3	  LightDir;
uniform float	SpecularCoeff;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec3 normal;
layout(location=3) in vec3 tangent;

out Data{
	vec3 fragPos;
    vec3 position;
    vec2 texcoord;
	vec3 normal;
	vec3 normalCameraSpace;
	float distToCamera;
} dataOut;

void main() {
    dataOut.fragPos = vec3( Model * vec4(position,1) );
	dataOut.position = position;
    dataOut.texcoord = texcoord;
    dataOut.normal = normal;
	dataOut.normalCameraSpace = vec3( Model * vec4(normal, 1) );

	vec4 cs_position = ModelViewProjection * Model * vec4(position,1);
    //= -cs_position.z;

	gl_Position = ModelViewProjection * Model * vec4(position,1);
	 dataOut.distToCamera = gl_Position.w;
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
in Data {
	vec3 fragPos;
    vec3 position;
    vec2 texcoord;
    vec3 normal;
	vec3 normalCameraSpace;
	float distToCamera;
} dataIn;

out vec4 out_color;

void main() {
	vec3 fragPos = dataIn.fragPos;
    vec3 vpos = dataIn.position;
    vec2 tcoord = dataIn.texcoord;
    vec3 normal = -normalize(dataIn.normal);

	vec3 n = dataIn.normal; //normalize(vec3( Model * vec4(-dataIn.normal, 1) ));

	vec3 lpos = vec3(100,100,0);
	vec3 l = normalize(lpos-fragPos);

	float ldot = max(dot(n,l),0.0);

	out_color = vec4(vec3(ldot),1);


}


#endif
// END
