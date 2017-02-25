/**
* Uniforms
*/



uniform sampler2D TextureMap;
uniform sampler2D shadowMap;
uniform sampler2D normalMap;

uniform mat4    ModelViewProjection;
uniform mat3	ModelView3x3;
uniform mat4	M; // Model Matrix
uniform mat4    V; // View Matrix
uniform mat4    InverseViewProjection;

uniform mat4	  DepthBiasMVP;
uniform vec3	  LightPos;
uniform vec4	  MaterialDiffuse;
uniform int		  ShadowBlur = 1;
uniform bool	  HasTexture = true;
uniform float     DepthBias = 0.01;
uniform float     AmbientFactor;
uniform float     MaterialShininess;
uniform float     LightIntensity;

#include "utils.glsl"

/**
* Vertex Shader
*
*/
#ifdef VERTEX_SHADER
layout(location=0) in vec3 position;
layout(location=1) in vec2 uv;
layout(location=2) in vec3 normal;
layout(location=3) in vec3 tangent;
layout(location=4) in vec3 bitangent;

out Data
{
    vec3 position;
    vec3 normal;
    vec4 ShadowCoord;
    vec3 LightPos;
    vec2 uv;

    vec3 Position_worldspace;
    vec3 EyeDirection_cameraspace;
    vec3 LightDirection_cameraspace;
    vec3 Normal_cameraspace;
    vec3 Tangent_cameraspace;
    vec3 Bitangent_cameraspace;
    vec3 LightDirection_tangentspace;
    vec3 EyeDirection_tangentspace;
    mat3 TBN;
    } dataOut;

void main()
{
    vec3 p = position;
    gl_Position =  ModelViewProjection * vec4(p,1);


    /** Calculate Stuff *************************************************************************************************/
    // Position of the vertex, in worldspace : M * position
    vec3 Position_worldspace = (M * vec4(position,1)).xyz;

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec3 vertexPosition_cameraspace = ( V * M * vec4(position,1)).xyz;
    vec3 EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 LightPosition_cameraspace = ( V * vec4(LightPos,1)).xyz;
    vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

    // Normal of the the vertex, in camera space
    vec3 Normal_cameraspace = ( V * M * vec4(normal,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

    dataOut.Position_worldspace=Position_worldspace;
    dataOut.EyeDirection_cameraspace = EyeDirection_cameraspace;
    dataOut.LightDirection_cameraspace=LightDirection_cameraspace;

    vec3 vertexNormal_cameraspace = ModelView3x3 * normalize(normal);
    vec3 vertexTangent_cameraspace = ModelView3x3 * normalize(tangent);
    vec3 vertexBitangent_cameraspace = ModelView3x3 * normalize(bitangent);

    dataOut.Normal_cameraspace=vertexNormal_cameraspace;
    dataOut.Tangent_cameraspace=vertexTangent_cameraspace;
    dataOut.Bitangent_cameraspace=vertexBitangent_cameraspace;

    mat3 TBN = transpose(mat3(
    vertexTangent_cameraspace,
    vertexBitangent_cameraspace,
    vertexNormal_cameraspace ));

    /** Set the DataOut *************************************************************************************************/

    dataOut.ShadowCoord = DepthBiasMVP * vec4(position,1);
    dataOut.position = position;
    dataOut.normal = normal;
    dataOut.LightPos = LightPos;
    dataOut.uv = uv;


    dataOut.LightDirection_tangentspace = TBN * LightDirection_cameraspace;
    dataOut.EyeDirection_tangentspace =  TBN * EyeDirection_cameraspace;
    dataOut.TBN = TBN;
}
#endif

#ifdef FRAGMENT_SHADER
/**
* Fragment Shader
*
*/

in Data {
vec3 position;
vec3 normal;
vec4 ShadowCoord;
vec3 LightPos;
vec2 uv;
vec3 Position_worldspace;
vec3 EyeDirection_cameraspace;
vec3 LightDirection_cameraspace;
vec3 Normal_cameraspace;
vec3 Tangent_cameraspace;
vec3 Bitangent_cameraspace;
vec3 LightDirection_tangentspace;
vec3 EyeDirection_tangentspace;
mat3 TBN;
} dataIn;

out vec4 out_color;

void main() {
    vec3 vpos = dataIn.position;
    vec4 ShadowCoord = dataIn.ShadowCoord;
    vec2 uv = dataIn.uv;

    ///
    ///
    // Convert screen coordinates to normalized device coordinates (NDC)
    float screen_width = iResolution.x;
    float screen_height = iResolution.y;
    float aspect = screen_width/screen_height;

    vec4 ndc = vec4(
      (gl_FragCoord.x / screen_width - 0.5) * 2.0,
      (gl_FragCoord.y / screen_height - 0.5) * 2.0,
      (gl_FragCoord.z - 0.5) * 2.0,
      1.0);

    // Convert NDC throuch inverse clip coordinates to view coordinates
    vec4 clip = InverseViewProjection * ndc;
    vec3 vertex = (clip / clip.w).xyz;
    ///

	// Shadow depth test
	float visibility = 1.0;
    float bias = DepthBias;
	float compare = (ShadowCoord.z-bias)/ShadowCoord.w;
	float fDepth = texture( shadowMap, (ShadowCoord.xy/ShadowCoord.w) ).z;
    if (  fDepth < compare  )
        visibility = 0.5;

	fDepth = compare / texture( shadowMap, (ShadowCoord.xy/ShadowCoord.w) ).z;
	visibility = max(0.45,PCF3( shadowMap, vec2(4096,4096), ShadowCoord.xy, ShadowCoord.z-bias, ShadowBlur));

	float diffuseFactor = dot(normalize(-dataIn.LightPos),dataIn.normal);
    vec3 diffuseColor = diffuseFactor * vec3(1);

    float ambientFactor = AmbientFactor;
	vec3 ambientColor = ambientFactor*vec3(1.0, 0.33, 0.01);

    // SpecularCoeff
    vec3 normal = dataIn.normal;
    vec3 surfaceToLight = normalize(LightPos-vertex);
    vec3 incidenceVector = -surfaceToLight; //a unit vector
    vec3 reflectionVector = reflect(incidenceVector, normal); //also a unit vector
    vec3 surfaceToCamera = normalize(iCameraPosition - vertex); //also a unit vector
    float cosAngle = max(0.0, dot(surfaceToCamera, reflectionVector));
    float materialShininess = MaterialShininess; // Plastic
    float specularCoefficient = 0.0;
    vec3 materialSpecularColor = vec3(1.0, 0.33, 0.01);
    float lightIntensity = LightIntensity;
    if(diffuseFactor > 0.0)
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), materialShininess);

    vec3 specularColor = specularCoefficient * materialSpecularColor * lightIntensity;
    // SpecularCoeff

	out_color = HasTexture ? texture(TextureMap, uv) : MaterialDiffuse;
    out_color = texture(TextureMap, uv)*MaterialDiffuse * min(1.0,diffuseFactor+0.5); //vec4(1) * visibility;
    out_color = MaterialDiffuse * min(1.0,diffuseFactor+0.5) * visibility;
   //out_color *= vec4((diffuseColor + ambientColor + specularColor),1) * visibility;
}


#endif
// END
