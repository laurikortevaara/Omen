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
out_color = texture(tex, tcoord); //vec4(1,1,1,1); //vec4(normal.x, normal.y, normal.z, 1);
	out_color = vec4(1);

	vec3 ldir = -vec3(0,150,0);
	vec3 lightDir = normalize(ldir - fragPos);

	vec3 viewDir = normalize(ViewPos - fragPos);
	vec3 reflectDir = reflect(lightDir, normal);  

	float specularStrength = 0.5; //Shininess*50;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec4 specular = specularStrength * spec * vec4(1);  
	
	float diff = max(dot(normal,lightDir), 0.0);
	vec4 diffuse = diff * vec4(1.0);

	vec4 ambient = vec4(0.2);
	vec4 objectColor = vec4(17,120,153,256)/256.0;
	//objectColor = texture(tex, tcoord);

	vec4 result = (ambient + diffuse + specular) * objectColor; //(ambient + diffuse + specular) * objectColor;
	out_color = result;
	out_color.a = objectColor.a;
	//out_color = vec4(abs(normal),1);
		
	//out_color.rgb /= sqrt(dataIn.distToCamera)*Shininess;
**/
/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec3 normal;

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
	
	vec3 n = normalize(vec3( Model * vec4(-dataIn.normal, 1) ));
	
	vec3 lpos = vec3(100,100,0);
	vec3 l = normalize(lpos-fragPos);

	float ldot = max(dot(n,l),0.0);

	out_color = vec4(vec3(ldot),1);
    
	
}


#endif
// END