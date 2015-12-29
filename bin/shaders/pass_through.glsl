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
 uniform mat4      ModelViewProjection;
 uniform sampler2D Texture;
 uniform sampler2D Texture2;
 uniform sampler2D Texture3;
 uniform sampler2D Texture4;
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
    //tcoord.y += Time*abs(sin(Time)*0.1);
    vec3 normal = dataIn.normal;
    out_color = vec4(1,0,0,1);
    out_color = out_color+vec4(tcoord.x, tcoord.y,0,1);
    out_color /= 2;
    out_color = vec4(tcoord.x, tcoord.y,0,1);
    //mat4x4 rot = rotationMatrix(vec3(0,0,1),Time);
    mat2x2 rot = mat2(cos(Time), sin(Time), -sin(Time), cos(Time));
    mat2x2 rot2 = mat2(cos(-Time), sin(-Time), -sin(-Time), cos(-Time));
    //out_color = mix(texture(Texture, (tcoord-0.5)*rot),texture(Texture2, tcoord*rot2),max(0,vpos.x));

    out_color = texture(Texture,tcoord);
    float len = length(vpos.xyz);
    out_color.xyz *= 3.0/len;
}


#endif
// END