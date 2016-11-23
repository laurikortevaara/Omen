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

out Data{
	smooth vec3 fragPos;
    flat vec3 position;
    smooth vec2 texcoord;
    flat vec3 normal;
	float distToCamera;
} dataOut;

void main() {
    dataOut.fragPos = vec3( Model * vec4(position,1) );
	dataOut.position = position;
    dataOut.texcoord = texcoord;
    dataOut.normal = normal;

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
	smooth vec3 fragPos;
    flat vec3 position;
    smooth vec2 texcoord;
    flat vec3 normal;
	float distToCamera;
} dataIn;

out vec4 out_color;

void main() {
	vec3 fragPos = dataIn.fragPos;
    vec3 vpos = dataIn.position;
    vec2 tcoord = dataIn.texcoord;
    vec3 normal = normalize(dataIn.normal);

    out_color = texture(tex, tcoord); //vec4(1,1,1,1); //vec4(normal.x, normal.y, normal.z, 1);
	out_color = vec4(1);

	vec3 lightDir = normalize(LightDir - fragPos);

	vec3 viewDir = normalize(ViewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);  

	float specularStrength = 0.5; //Shininess*50;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * vec3(1);  
	
	float diff = max(dot(normal,lightDir), 0.0);
	vec3 diffuse = diff * vec3(1.0);

	vec3 ambient = vec3(1.0);
	vec3 objectColor = texture(tex, tcoord).rgb;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	out_color = vec4(result, 1.0f);
	out_color.a = texture(tex, tcoord).a;
	out_color.rgb /= sqrt(dataIn.distToCamera)*Shininess;
	out_color = texture(tex,tcoord);
	//out_color = vec4(normal,1);
}


#endif
// END