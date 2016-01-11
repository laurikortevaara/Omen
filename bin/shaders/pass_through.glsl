mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 ExtractCameraPos_NoScale(mat4 ModelView)
{
   return ModelView[3].xyz;
}

/**
 * Uniforms
 */
uniform sampler2D Texture;
uniform sampler2DShadow ShadowMap;
uniform bool      TextureEnabled;
uniform bool      MetacapTextureEnabled;
uniform mat4      Model;
uniform mat4      ModelViewProjection;
uniform mat4      ModelView;
uniform mat4      View;
uniform mat4      ModelViewInverse;
uniform mat4      NormalMatrix;
uniform mat4      DepthBiasMVP;
uniform vec3      LightPosition;
uniform vec3      LightColor;
uniform float     LightIntensity;


uniform float     Time;


uniform vec4      AmbientColor;
uniform vec4      DiffuseColor;
uniform vec4      SpecularColor;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec4 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec3 normal;

out Data{
    vec4 position;
    vec2 texcoord;
    vec3 normal;
    vec4 shadow_coord;
    vec3 position_worldspace;
    vec3 eye_direction_cameraspace;
    vec3 light_direction_cameraspace;
    vec3 normal_cameraspace;
} dataOut;

void main() {
    dataOut.position = position;
    dataOut.texcoord = texcoord;
    dataOut.normal = normalize(Model*vec4(normal,1)).xyz;
    dataOut.shadow_coord = DepthBiasMVP * position;
    dataOut.position_worldspace = (Model * position).xyz;

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    dataOut.eye_direction_cameraspace = vec3(0,0,0) - ( ModelView * position).xyz;

    // Vector that goes from the vertex to the light, in camera space
    dataOut.light_direction_cameraspace = (View*vec4(vec3(-LightPosition),0)).xyz;

    // Normal of the the vertex, in camera space
    dataOut.normal_cameraspace = ( ModelView * vec4(normal,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

    gl_Position = ModelViewProjection * Model * position;
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
in Data {
    vec4 position;
        vec2 texcoord;
        vec3 normal;
        vec4 shadow_coord;
        vec3 position_worldspace;
        vec3 eye_direction_cameraspace;
        vec3 light_direction_cameraspace;
        vec3 normal_cameraspace;
} dataIn;

out vec4 out_color;

void main() {
    vec4 vpos = ModelView*dataIn.position;
    vec2 tcoord = dataIn.texcoord;
    vec3 normal = dataIn.normal; //normalize(mat3(NormalMatrix)*dataIn.normal);

    float len = length(vpos.xyz);
    if(TextureEnabled){
        out_color = texture(Texture,tcoord);
        out_color /= len;
        }


    float visibility = 1.0;
    //visibility = texture( ShadowMap, vec3(dataIn.shadow_coord.xy, (dataIn.shadow_coord.z)/dataIn.shadow_coord.w) );


    out_color = texture(Texture, tcoord);
    //out_color = vec4(mod(tcoord.x,1), mod(tcoord.y,1.0), 0, 1 );
    len = length(dataIn.position_worldspace - LightPosition);
    float brightness = max(0.15,dot(normal, LightPosition)*(1/len));

    //out_color = mix(AmbientColor, out_color*brightness, 0.5 );

    if(MetacapTextureEnabled){
       float radius = length(normal);
       float theta = atan(normal.y, normal.x);
       float phi = acos(normal.z / radius);
       out_color = vec4(theta, phi, 0, 1);//texture(Texture2, )
       out_color = texture(Texture, vec2(0.5+cos(theta),0.5+sin(phi)) );
    }


    out_color *= brightness;
    out_color.a = 1;
}


#endif
// END