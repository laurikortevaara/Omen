//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include "Mesh.h"
#include "OpenGL/gl3.h"
#include "GL_error.h.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

#define BUFFER_OFFSET(offset)  (0 + offset)

GLuint vao = 0;
GLuint vbo = 0;
GLuint vbo_barycentric = 0;
GLuint shader_program = 0;

void loadShader( const std::string& filename, std::string& out ) {
    std::string line;
    std::ifstream in(filename.c_str());
    while(std::getline(in, line))
    {
        out += line + "\n";
    }
}

void Mesh::loadShaders()
{
    std::string vertex_shader; loadShader("vs.glsl", vertex_shader);
    std::string control_shader; loadShader("tc.glsl", control_shader);
    std::string eval_shader; loadShader("te.glsl", eval_shader);
    std::string geom_shader; loadShader("gs.glsl", geom_shader);
    std::string fragment_shader; loadShader("fs.glsl", fragment_shader);


    GLint sizei = 255;
    GLchar log1[255];
    GLchar log2[255];
    GLchar log3[255];
    GLchar log4[255];
    GLchar log5[255];

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* ss = vertex_shader.c_str();
    glShaderSource(vs, 1, &ss, NULL);
    glCompileShader(vs);
    glGetShaderInfoLog(vs,255, &sizei, log1);

    GLuint tc = glCreateShader(GL_TESS_CONTROL_SHADER);
    ss = control_shader.c_str();
    glShaderSource(tc, 1, &ss, NULL);
    glCompileShader(tc);
    glGetShaderInfoLog(tc,255, &sizei, log2);

    GLuint te = glCreateShader(GL_TESS_EVALUATION_SHADER);
    ss = eval_shader.c_str();
    glShaderSource(te, 1, &ss, NULL);
    glCompileShader(te);
    glGetShaderInfoLog(te,255, &sizei, log3);

    GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
    ss = geom_shader.c_str();
    glShaderSource(gs, 1, &ss, NULL);
    glCompileShader(gs);
    glGetShaderInfoLog(gs,255, &sizei, log4);


    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    ss = fragment_shader.c_str();
    glShaderSource(fs, 1, &ss, NULL);
    glCompileShader(fs);
    glGetShaderInfoLog(fs,255, &sizei, log5);

    assert(strlen(log1)==0);
    assert(strlen(log2)==0);
    assert(strlen(log3)==0);
    assert(strlen(log4)==0);
    assert(strlen(log5)==0);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vs);
    //glAttachShader(shader_program, tc);
    //glAttachShader(shader_program, te);
    //glAttachShader(shader_program, gs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);
}

void Mesh::createMesh()
{
    glGenVertexArrays(1, &vao);
    check_gl_error();
    glBindVertexArray(vao);
    check_gl_error();

    GLint inputVertices = glGetAttribLocation(shader_program, "vPosition" );
    GLint inputBaryCentric = glGetAttribLocation(shader_program, "Barycentric_VS_in" );

    if(inputVertices>=0) {
        float points[] = {
                -1.0f, -1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                /*-1.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,
                1.0f, -1.0f, 0.0f*/
        };

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
        check_gl_error();
        glEnableVertexAttribArray(inputVertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        check_gl_error();
        glVertexAttribPointer(inputVertices, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        check_gl_error();
    }

    if(inputBaryCentric>=0) {

        float barycentric[] = {
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f,
                /*1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f,*/
        };

        glGenBuffers(1, &vbo_barycentric);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_barycentric);
        glBufferData(GL_ARRAY_BUFFER, sizeof(barycentric), barycentric, GL_STATIC_DRAW);
        check_gl_error();

        glEnableVertexAttribArray(inputBaryCentric);
        check_gl_error();

        glBindBuffer(GL_ARRAY_BUFFER, vbo_barycentric);
        check_gl_error();
        glVertexAttribPointer(inputBaryCentric, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        check_gl_error();
    }
}

Mesh::Mesh()
{
    loadShaders();
    createMesh();
}

Mesh::~Mesh()
{

}

void Mesh::render()
{
    glUseProgram(shader_program);
    check_gl_error();
    {
        ////
        //// Tessellation parameters
        ////
        /*
        GLint tessLevelInner = glGetUniformLocation(shader_program, "innerTess");
        check_gl_error();

        GLint tessLevelOuter = glGetUniformLocation(shader_program, "outerTess");
        check_gl_error();

        glUniform1f(tessLevelInner, 1);
        glUniform1f(tessLevelOuter, 1);
        check_gl_error();
        */

        ////
        //// Projection Matrix
        ////
        GLint mvp = glGetUniformLocation(shader_program, "ModelViewProjectionMatrix");
        if(mvp>=0)
        {
            glm::mat4 mvpi = glm::mat4(1.0);
            mvpi = glm::translate(mvpi, glm::vec3(0,0,0));
            check_gl_error();
            glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(mvpi));
            check_gl_error();
        }


        ////
        //// Drawing the mesh
        ////
        {
            glBindVertexArray(vao);
            check_gl_error();

/*
            GLint pos_in = glGetAttribLocation(shader_program, "Position_VS_in");
            glEnableVertexAttribArray(pos_in);
            check_gl_error();
*/
            glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
            check_gl_error();
        }
    }
    // glPatchParameteri(GL_PATCH_VERTICES, 3);
   // glDrawArrays( GL_PATCHES, 0, 3);
}

