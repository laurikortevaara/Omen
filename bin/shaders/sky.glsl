uniform mat4 ViewMatrix;

const float PI = 3.14159265359;

#ifdef VERTEX_SHADER
layout(location=0) in vec3 position;

out vec3 vposition;

void main() {
    vposition = position;
    gl_Position = vec4(position,1);
}
#endif
#ifdef FRAGMENT_SHADER

in vec3 vposition;
out vec4 color;
void main() {
    vec3 pos = vec3(ViewMatrix*vec4(vposition,1));

    float f = abs(sin(vposition.y*PI/2+PI/2));
    float step1 = 0.013;
    float step2 = 0.533;
    float step3 = 0.933;
    vec4 white = vec4(0.110,0.145,0.200,1.00);
    vec4 red = vec4(0.584,0.596,0.620,1.00);
    vec4 blue = vec4(0.898,0.859,0.749,1.00);
    vec4 green = vec4(1.000,0.890,0.647,1.00);
    color = mix(white, red, smoothstep(step1, step2, f));
    color = mix(color, blue, smoothstep(step2, step3, f));
    color = mix(color, green, smoothstep(step3, 1, f));
}
#endif