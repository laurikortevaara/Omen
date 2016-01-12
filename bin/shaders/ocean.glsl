uniform mat4 ViewMatrix;
uniform mat4 ModelViewProjection;

const float PI = 3.14159265359;

#ifdef VERTEX_SHADER
layout(location=0) in vec3 position;

out vec3 vposition;

void main() {
    vposition = position;
    gl_Position = ModelViewProjection*vec4(position,1);
}
#endif
#ifdef FRAGMENT_SHADER

in vec3 vposition;
out vec4 color;
void main() {
    vec3 pos = vec3(ViewMatrix*vec4(vposition,1));

    color = vec4(0.5,0.7,0.8,1.0);
}
#endif