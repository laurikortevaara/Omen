
#ifdef VERTEX_SHADER
layout(location=0) in vec3 vertexIn;

void main() {
   gl_Position = vec4(vertexIn,1.0);
}
#endif

#ifdef FRAGMENT_SHADER
out vec4 fragColor;

void main()
{
  fragColor = vec4(1,0,0,1);
}
#endif
