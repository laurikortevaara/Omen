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

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
layout(location=0) in vec3 position;
layout(location=2) in vec3 normal;
layout(location=5) in vec3 barycentric;

out Data{
  vec3 position;
  smooth vec3 normal;
  float vertExcludeEdge;
} dataOut;

void main() {
  gl_Position = ModelViewProjection * vec4(position,1);
  dataOut.position = position;
  dataOut.normal = normal;
  dataOut.vertExcludeEdge = 0;
}
#endif

#ifdef GEOMETRY_SHADER
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in Data {
  vec3 position;
  smooth vec3 normal;
  float vertExcludeEdge;
} dataIn[];

out Data {
  vec3 position;
  smooth vec3 normal;
  noperspective vec3 dist;
} dataOut;

void main() {
    float MEW = 1000.0;
  //iResolution
  vec2 res = iResolution;
  // taken from 'Single-Pass Wireframe Rendering'
    vec2 p0 = res * gl_in[0].gl_Position.xy/ gl_in[0].gl_Position.w;
    vec2 p1 = res * gl_in[1].gl_Position.xy/ gl_in[1].gl_Position.w;
    vec2 p2 = res * gl_in[2].gl_Position.xy/ gl_in[2].gl_Position.w;
    vec2 v0 = p2-p1;
    vec2 v1 = p2-p0;
    vec2 v2 = p1-p0;
    float area = abs(v1.x*v2.y - v1.y * v2.x);

    dataOut.dist = vec3(area/length(v0),dataIn[1].vertExcludeEdge*MEW,dataIn[2].vertExcludeEdge*MEW);
    dataOut.position = dataIn[0].position;
    dataOut.normal = dataIn[0].normal;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    dataOut.dist = vec3(dataIn[0].vertExcludeEdge*MEW,area/length(v1),dataIn[2].vertExcludeEdge*MEW);
    dataOut.position = dataIn[1].position;
    dataOut.normal = dataIn[1].normal;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    dataOut.dist = vec3(dataIn[0].vertExcludeEdge*MEW,dataIn[1].vertExcludeEdge*MEW,area/length(v2));
    dataOut.position = dataIn[2].position;
    dataOut.normal = dataIn[2].normal;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
in Data {
  vec3 position;
  smooth vec3 normal;
  noperspective vec3 dist;
} dataIn;

out vec4 out_color;

void main() {
	vec3 vpos = dataIn.position;
	vec3 normal = normalize(dataIn.normal);
    vec3 dist = dataIn.dist;

    vec3 light = normalize(vec3(ViewMatrix*vec4(0,1,1,0)));
    float d = dot(-light,normal);

    float nearD = min(min(dist[0],dist[1]),dist[2]);
    float edgeIntensity = exp2(-1.0*nearD*nearD);

     out_color = (edgeIntensity * vec4(0.1,0.1,0.1,1.0)) + d*((1.0-edgeIntensity) * vec4(1,0,0,1));
}


#endif
// END
