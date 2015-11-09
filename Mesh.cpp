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

#define BUFFER_OFFSET(offset)  (0 + offset)

GLuint vao = 0;
GLuint vbo = 0;
GLuint vbo_barycentric = 0;
GLuint shader_programme = 0;

Mesh::Mesh() {

   /* std::shared_ptr<Triangle> t = std::make_shared<Triangle>(
            Triangle(
                    Vertex(-1.0f,-1.0f,0.0f),
                    Vertex( 1.0f,-1.0f,0.0f),
                    Vertex( 0.0f, 1.0f,0.0f)
            )
            );
    m_triangles.push_back(t);

    check_gl_error();

    m_vbo = std::make_shared<VertexBufferObject>(VertexBufferObject());
    m_vbo->setMesh(m_triangles);
    m_vbo->deactivate();
    check_gl_error();

    m_vao = std::make_shared<VertexArrayObject>(VertexArrayObject());
    GLint vao = m_vao->m_index;
    glBindVertexArray(vao);
    check_gl_error();
    GLint va = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &va);

    check_gl_error();
    glEnableVertexAttribArray(0);
    check_gl_error();
    glBindBuffer (GL_ARRAY_BUFFER, m_vbo->m_index);
    check_gl_error();
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    check_gl_error();*/

    float points[] = {
            0.0f,  0.5f,  0.0f,
            0.5f, -0.5f,  0.0f,
            -0.5f, -0.5f, 0.0f
    };

    float barycentric[] = {
            1.0f,  0.0f,  0.0f,
            0.0f,  1.0f,  0.0f,
            0.0f,  0.0f,  1.0f,
    };


    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), points, GL_STATIC_DRAW);
    check_gl_error();

    glGenBuffers(1, &vbo_barycentric);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_barycentric);
    glBufferData (GL_ARRAY_BUFFER, 9 * sizeof (float), barycentric, GL_STATIC_DRAW);
    check_gl_error();

    glGenVertexArrays(1, &vao);
    check_gl_error();
    glBindVertexArray(vao);
    check_gl_error();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    check_gl_error();

    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    check_gl_error();
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    check_gl_error();

    glBindBuffer (GL_ARRAY_BUFFER, vbo_barycentric);
    check_gl_error();
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    check_gl_error();

    const char* vertex_shader =
            "#version 400\n"
                    "uniform mat4 mvp;"
                    "in vec3 vp;"
                    "in vec3 barycentric;"

                    "out vec3 vBC;"
                    "void main () {"
                    "  vBC = barycentric;"
                    "  gl_Position = mvp * vec4 (vp, 1.0);"
                    "}";

    const char* fragment_shader =
            "#version 400\n"
                    "in vec3 vBC;"
                    "out vec4 frag_colour;"
                    "uniform vec4 argbi;"
                    "void main () {"
                    "  vec4 argb = argbi;"
                    "  if(any(lessThan(vBC, vec3(0.005)))){"
                    "    argb = vec4(0.0, 0.0, 0.0, 1.0);"
                    "   }"
                    "  frag_colour = argb;"
                    "}";

    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vertex_shader, NULL);
    glCompileShader (vs);
    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &fragment_shader, NULL);
    glCompileShader (fs);

    shader_programme = glCreateProgram ();
    glAttachShader (shader_programme, fs);
    glAttachShader (shader_programme, vs);
    glLinkProgram (shader_programme);
}

Mesh::~Mesh() {

}

void Mesh::render() {
    glUseProgram (shader_programme);
    glm::mat4 mvpi = glm::mat4(1.0);
    static GLfloat angle = 0.0f;
    angle += 0.0f;
    mvpi = glm::rotate( mvpi, angle, glm::vec3(0.0,1.0,0.0));

    GLint mvp = glGetUniformLocation(shader_programme,"mvp");
    GLint loca = glGetUniformLocation(shader_programme,"argbi");
    glUniform4f(loca, 1.0f,1.0f,1.0f,1.0f);
    glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(mvpi));
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

