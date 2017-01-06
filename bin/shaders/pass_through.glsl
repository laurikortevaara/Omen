/**
 * Uniforms
 */
uniform sampler2D tex;
uniform mat4      ModelViewProjection;
uniform mat4      ViewMatrix;
uniform mat4      ViewProjMatrix;
uniform mat4      ModelMatrix;
uniform mat4      ModelViewMatrix;
uniform mat3      NormalMatrix;
uniform vec3	  ViewPos;
uniform float	  Shininess;
uniform vec3	  LightDir;
uniform float	  SpecularCoeff;
uniform vec4	  MaterialDiffuse;
uniform vec4	  MaterialSpecular;
uniform vec4	  MaterialAmbient;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec3 normal;
layout(location=3) in vec3 tangent;
layout(location=4) in vec3 bitangent;

out Data{
  vec3 fragPosCameraSpace;
  vec3 position;
  vec2 texcoord;
    smooth vec3 normal;
    vec3 normalCameraSpace;
  vec3 tangentCameraSpace;
  vec3 bitangentCameraSpace;
    float distToCamera;
  vec3 LightPosition_cameraspace;
  vec3 LightDirection_cameraspace;
  vec3 EyeDirection_cameraspace;
} dataOut;

void main() {
    gl_Position = ModelViewProjection * vec4(position,1);

  // Vector that goes from the vertex to the camera, in camera space.
  // In camera space, the camera is at the origin (0,0,0).
  vec3 vertexPosition_cameraspace = gl_Position.xyz;
  dataOut.EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

  vec3 LightPosition_worldspace = vec3(100,100,-100);
    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
  dataOut.LightPosition_cameraspace = ( ViewMatrix * vec4(LightPosition_worldspace,1)).xyz;
  dataOut.LightDirection_cameraspace = dataOut.LightPosition_cameraspace + dataOut.EyeDirection_cameraspace;

  dataOut.fragPosCameraSpace = vec3( ModelMatrix * vec4(position,1) );
    dataOut.position = position;
  dataOut.texcoord = texcoord;
  dataOut.normal = normal;
    dataOut.normalCameraSpace = vec3( NormalMatrix * normal );
  dataOut.tangentCameraSpace = vec3( NormalMatrix * tangent );
  dataOut.bitangentCameraSpace = vec3( NormalMatrix * bitangent );
    dataOut.distToCamera = gl_Position.w;
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
in Data {
  vec3 fragPosCameraSpace;
  vec3 position;
  vec2 texcoord;
    smooth vec3 normal;
    vec3 normalCameraSpace;
  vec3 tangentCameraSpace;
  vec3 bitangentCameraSpace;
    float distToCamera;
  vec3 LightPosition_cameraspace;
  vec3 LightDirection_cameraspace;
  vec3 EyeDirection_cameraspace;
} dataIn;

out vec4 out_color;

void main() {
	vec3 fragPos = dataIn.fragPosCameraSpace;
	vec3 vpos = dataIn.position;
	vec2 tcoord = dataIn.texcoord;
	vec3 normal = dataIn.normal;
    vec3 n = dataIn.normalCameraSpace; //normalize(vec3( Model * vec4(-dataIn.normal, 1) ));
	vec3 tangentCameraSpace = dataIn.tangentCameraSpace;
	vec3 bitangentCameraSpace = dataIn.bitangentCameraSpace;
	vec3 normalCameraSpace = dataIn.normalCameraSpace;

	mat3 TBN = transpose(mat3(
      tangentCameraSpace,
      bitangentCameraSpace,
      normalCameraSpace
	));

    vec3 lpos = vec3(100,100,0);
    vec3 l = normalize(lpos-fragPos);

	vec3 LightDirection_tangentspace = TBN * dataIn.LightDirection_cameraspace;
	vec3 EyeDirection_tangentspace =  TBN * dataIn.EyeDirection_cameraspace;

    float ldot = abs(dot(normal,l));

	out_color = /*ldot **/ texture(tex,tcoord);
	out_color = vec4(tcoord.x,tcoord.y,0,1);
	out_color.a = 1;
}


#endif
// END
