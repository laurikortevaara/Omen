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
uniform mat4	  depthMVP;

/**
 * Vertex Shader
 *
 */
#ifdef VERTEX_SHADER
#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;

void main(){
 gl_Position =  depthMVP * vec4(vertexPosition_modelspace,1);
}
#endif

#ifdef FRAGMENT_SHADER
/**
 * Fragment Shader
 *
 */
// Ouput data
layout(location = 0) out float fragmentdepth;

void main(){
    // Not really needed, OpenGL does it anyway
    fragmentdepth = gl_FragCoord.z;
}

#endif
// END
