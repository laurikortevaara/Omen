/**
 * Uniforms
 */
uniform sampler2D tex;
uniform mat4      ModelViewProjection;
uniform mat4      Model;
uniform vec3	  ViewPos;
uniform float	  Shininess;

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
    flat vec3 normal;
} dataOut;

void main() {
    dataOut.fragPos = vec3( Model * vec4(position,1) );
	dataOut.position = position;
    dataOut.texcoord = texcoord;
    dataOut.normal = normal;

	gl_Position = ModelViewProjection * Model * vec4(position,1);
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
    flat vec3 normal;
} dataIn;

out vec4 out_color;

void main() {
	vec3 fragPos = dataIn.fragPos;
    vec3 vpos = dataIn.position;
    vec2 tcoord = dataIn.texcoord;
    vec3 normal = normalize(dataIn.normal);

    out_color = texture(tex, tcoord); //vec4(1,1,1,1); //vec4(normal.x, normal.y, normal.z, 1);
	out_color = vec4(1);

	vec3 lightDir = normalize(vec3(0,1,0) - fragPos);

	vec3 viewDir = normalize(ViewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);  

	float specularStrength = Shininess*50;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512);
	vec3 specular = specularStrength * spec * vec3(1);  
	
	float diff = max(dot(normal,lightDir), 0.0);
	vec3 diffuse = diff * vec3(0.5);

	vec3 ambient = vec3(0.01);
	vec3 objectColor = 2*texture(tex, tcoord).rgb;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	out_color = vec4(result, 1.0f);
	out_color.a = texture(tex, tcoord).a;

}


#endif
// END