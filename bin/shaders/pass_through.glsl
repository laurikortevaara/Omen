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
 uniform bool      TextureEnabled;
 uniform mat4      ModelViewProjection;
 uniform mat4      ModelView;
 uniform mat4      ModelViewInverse;
 uniform mat4      NormalMatrix;
 uniform sampler2D Texture;
 uniform sampler2D Texture2;
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
} dataOut;

void main() {
    dataOut.position = position;
    dataOut.texcoord = texcoord;
    dataOut.normal = normal;

    gl_Position = ModelViewProjection * position;
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
} dataIn;

out vec4 out_color;

void main() {
    vec4 vpos = dataIn.position;
    vec2 tcoord = dataIn.texcoord;
    vec3 normal = normalize(mat3(NormalMatrix)*dataIn.normal);

    float len = length(vpos);
    if(TextureEnabled)
        out_color = texture(Texture,tcoord);
    else {
        float radius = length(normal);
        float theta = atan(normal.y, normal.x);
        float phi = acos(normal.z / radius);
        out_color = vec4(theta, phi, 0, 1);//texture(Texture2, )
        }
    out_color /= len;
    out_color.a = 1;
}


#endif
// END