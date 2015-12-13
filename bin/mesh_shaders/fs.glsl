#version 410 core

uniform int RenderWireframe;
uniform mat3 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;
uniform sampler2D tex;

in Data{
    vec3 normal;
    vec4 eye;
    vec2 texcoord;
} DataIn;

out vec4 colorOut;


void main() 
{
    // Light direction
    vec3 l_dir = vec3(0,0,1);
    vec4 diffuse = vec4(1.0,0.0,0.0,1.0);
    vec4 ambient = vec4(0.1,0.1,0.1,1.0);
    vec4 specular = vec4(1);
    float shininess = 0.5;

    // set the specular term to black
    vec4 spec = vec4(0.0);

    // normalize both input vectors
    vec3 n = normalize(DataIn.normal);
    vec3 e = normalize(vec3(DataIn.eye));

    float intensity = max(dot(n,l_dir), 0.0);

    // if the vertex is lit compute the specular color
    if (intensity > 0.0) {
        // compute the half vector
        vec3 h = normalize(l_dir + e);
        // compute the specular term into spec
        float intSpec = max(dot(h,n), 0.0);
        spec = specular * pow(intSpec,shininess);
    }
    colorOut = max(intensity *  diffuse + spec, ambient);
}