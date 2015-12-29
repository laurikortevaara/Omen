//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include "Mesh.h"
#include <GLFW/glfw3.h>
#include "GL_error.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Texture.h"
#include "utils.h.h"
#include "Engine.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <tinydir.h>
#include <algorithm>
#include <time.h>

using namespace Omen;

#define BUFFER_OFFSET(offset)  (0 + offset)

GLuint vao = 0;
GLuint vbo = 0;
GLuint vbo_barycentric = 0;
GLuint vbo_texcoord = 0;
GLint attrib_vPosition;
GLint attrib_barycentric;
GLint attrib_texcoord;

GLboolean draw_mesh = 1;
GLboolean draw_triangle_patches = 1;

Mesh::Mesh() : fInnerTess(1.0),
               fOuterTess(1.0),
               mPolygonMode(GL_LINE),
               m_shader(nullptr),
               m_material(nullptr),
               m_position(0),
               m_amplitude(Omen::random(-10,10)*0.05),
               m_phase(3.14*Omen::random(0,100)/100.0),
               m_frequency(0.5+Omen::random(0,100)/100.0){
    m_material = new Material;
    std::string texture_file = "cat.jpg";

    static std::vector<std::string> files;
    tinydir_dir dir;
    std::string filename;
    std::vector<std::string> accepted_exts = {".jpg", ".png"};
    if(files.empty())
    if (tinydir_open(&dir, "./textures") != -1)
        while (dir.has_next) {
            tinydir_file file;
            if (tinydir_readfile(&dir, &file) != -1) {
                if (!file.is_dir) {
                    filename.assign((const char*)file.name);
                    std::string fname_lower = filename;
                    std::transform(fname_lower.begin(), fname_lower.end(), fname_lower.end(), ::tolower);
                    for (std::string ext : accepted_exts) {
                        if (fname_lower.find(ext) != std::string::npos)
                            files.push_back(std::string((const char*)file.path));
                    }
                }
            }
            tinydir_next(&dir);
        }

    texture_file = files.empty() ? "cat.jpg" : files[Omen::random(0,files.size()-1)];
    m_material->setTexture(new Texture(texture_file));
    m_shader = new Shader("shaders/pass_through.glsl");


    /**/
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    check_gl_error();

    /**
     * Setup the vertex coordinate buffer object (vbo)
     */
    GLfloat s = 2;
    // Enable vertex attributes
    m_vcoord_attrib = m_shader->getAttribLocation("position");
    if (m_vcoord_attrib >= 0) {
        glEnableVertexAttribArray(m_vcoord_attrib);

        GLfloat vertices[4][3] = {
                {-s, 2, -s},
                {s,  2, -s},
                {s,  2, s},
                {-s, 2, s}};

        int i = sizeof(vertices);
        // Create vbo
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(m_vcoord_attrib, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                              0/*stride*/, 0/*offset*/);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        check_gl_error();

        /**
         * Setup the vertex index element buffer (ibo)
         */
        GLuint indices[6] = {0, 1, 3, 1, 2, 3};
        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        check_gl_error();
    }

    /**
     * Setup the vertex texture coordinates
     */
    m_tcoord_attrib = m_shader->getAttribLocation("texcoord");
    check_gl_error();
    if (m_tcoord_attrib >= 0) {
        check_gl_error();
        GLfloat texcoords[4][2] = {
                {0, 0},
                {1, 0},
                {1, 1},
                {0, 1}
        };
        glGenBuffers(1, &m_vbo_texcoord);
        check_gl_error();
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoord);
        check_gl_error();
        glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
        check_gl_error();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        check_gl_error();
    }
    /**/
}

Mesh::~Mesh() {

}

/*
void Mesh::loadShaders() {
    shader_program = glCreateProgram();

    std::string vertex_shader = "";
    std::string control_shader = "";
    std::string eval_shader = "";
    std::string geom_shader = "";
    std::string fragment_shader = "";
    if (draw_mesh) {
        loadShader("mesh_shaders/vs.glsl", vertex_shader);
        loadShader("mesh_shaders/fs.glsl", fragment_shader);
    }
    else {
        loadShader("patch_shaders/vs.glsl", vertex_shader);
        if (draw_triangle_patches) {
            loadShader("patch_shaders/tc_triangle.glsl", control_shader);
            loadShader("patch_shaders/te_triangle.glsl", eval_shader);
        }
        else {
            loadShader("patch_shaders/tc_quad.glsl", control_shader);
            loadShader("patch_shaders/te_quad.glsl", eval_shader);
            //loadShader("patch_shaders/gs.glsl", geom_shader);
        }
        loadShader("patch_shaders/fs.glsl", fragment_shader);
    }


    GLint sizei = 255;
    GLchar log1[255] = {""};
    GLchar log2[255] = {""};
    GLchar log3[255] = {""};
    GLchar log4[255] = {""};
    GLchar log5[255] = {""};
    GLchar log_program[255];

    if (!vertex_shader.empty()) {
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        const GLchar *ss = vertex_shader.c_str();
        glShaderSource(vs, 1, &ss, NULL);
        glCompileShader(vs);
        glGetShaderInfoLog(vs, 255, &sizei, log1);
        std::cout << log1 << std::endl;
        assert(strlen(log1) == 0);
        glAttachShader(shader_program, vs);
    }

    if (!control_shader.empty()) {
        GLuint tc = glCreateShader(GL_TESS_CONTROL_SHADER);
        const GLchar *ss = control_shader.c_str();
        glShaderSource(tc, 1, &ss, NULL);
        glCompileShader(tc);
        glGetShaderInfoLog(tc, 255, &sizei, log2);
        std::cout << log2 << std::endl;
        assert(strlen(log2) == 0);
        glAttachShader(shader_program, tc);
        check_gl_error();
    }

    if (!eval_shader.empty()) {
        GLuint te = glCreateShader(GL_TESS_EVALUATION_SHADER);
        const GLchar *ss = eval_shader.c_str();
        glShaderSource(te, 1, &ss, NULL);
        glCompileShader(te);
        glGetShaderInfoLog(te, 255, &sizei, log3);
        std::cout << log3 << std::endl;
        assert(strlen(log3) == 0);
        glAttachShader(shader_program, te);
    }

    if (!geom_shader.empty()) {

        GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
        const GLchar *ss = geom_shader.c_str();
        glShaderSource(gs, 1, &ss, NULL);
        glCompileShader(gs);
        glGetShaderInfoLog(gs, 255, &sizei, log4);
        std::cout << log4 << std::endl;
        assert(strlen(log4) == 0);
        glAttachShader(shader_program, gs);
        check_gl_error();
    }

    if (!fragment_shader.empty()) {
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        const GLchar *ss = fragment_shader.c_str();
        glShaderSource(fs, 1, &ss, NULL);
        glCompileShader(fs);
        glGetShaderInfoLog(fs, 255, &sizei, log5);
        assert(strlen(log5) == 0);
        glAttachShader(shader_program, fs);
        check_gl_error();
    }

    glLinkProgram(shader_program);
    glGetProgramInfoLog(shader_program, 255, &sizei, log_program);
    check_gl_error();
    assert(strlen(log5) == 0);
}

void Mesh::createPatches() {
    attrib_vPosition = glGetAttribLocation(shader_program, "vPosition");
    attrib_barycentric = glGetAttribLocation(shader_program, "barycentric");
    attrib_texcoord = glGetAttribLocation(shader_program, "texcoord");

    ///
    /// Create Vertex Array Object
    ///
    glGenVertexArrays(1, &vao);
    check_gl_error();
    glBindVertexArray(vao);
    check_gl_error();

    ///
    /// Create Array Buffer for vertex coordinates
    ///
    const GLvoid *vertices = draw_triangle_patches ? vertices_tri : vertices_quad;
    const GLvoid *baryCoords = draw_triangle_patches ? baryCoord_tri : baryCoords_quad;
    const GLvoid *uvCoords = draw_triangle_patches ? uvCoords_tri : uvCoords_quad;
    int num_elements = draw_triangle_patches ? 2 * 3 : 4;

    if (attrib_vPosition >= 0) {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, num_elements * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    }

    ///
    /// Create Array Buffer for barycentric coordinates
    ///
    if (attrib_barycentric >= 0) {
        glGenBuffers(1, &vbo_barycentric);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_barycentric);
        glBufferData(GL_ARRAY_BUFFER, num_elements * 3 * sizeof(GLfloat), baryCoords, GL_STATIC_DRAW);
    }

    ///
    /// Create Array Buffer for texture coordinates
    ///
    if (attrib_texcoord >= 0) {
        // Init resources
        glGenBuffers(1, &vbo_texcoord);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
        glBufferData(GL_ARRAY_BUFFER, num_elements * 2 * sizeof(GLfloat), uvCoords, GL_STATIC_DRAW);
    }

}
*/

void Mesh::render(const glm::mat4 &viewProjection) {
    //glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode);

    Engine* e = Engine::instance();

    m_position.y = (float) (m_amplitude * sin(e->time()*m_frequency + m_phase));

    glBindVertexArray(m_vao);
    if (m_shader == nullptr)
        return;
    m_shader->use();

    check_gl_error();
    {
        ////
        //// Tessellation parameters
        ////
        m_shader->setUniform1f("innerTess", fInnerTess);
        m_shader->setUniform1f("outerTess", fOuterTess);
        check_gl_error();

        ////
        //// Projection Matrix
        ////
        glm::mat4 ModelViewProjection = viewProjection * m_transform.tr();
        m_shader->setUniformMatrix4fv("ModelViewProjection", 1, &ModelViewProjection[0][0], false);

        ////
        //// Drawing the mesh
        ////
        {
            m_shader->setUniform1f("useTexture", m_use_texture);

            ////
            //// Material
            ////
            if (m_material != nullptr)
                m_shader->setMaterial(m_material);

            // Render
            if (attrib_barycentric >= 0) {
                glBindBuffer(GL_ARRAY_BUFFER, vbo_barycentric);
                check_gl_error();
                glEnableVertexAttribArray(attrib_barycentric);
                check_gl_error();
                glVertexAttribPointer(attrib_barycentric, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                check_gl_error();
            }
            /*
            if (attrib_vPosition >= 0) {
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                check_gl_error();
                glEnableVertexAttribArray(attrib_vPosition);
                check_gl_error();
                glVertexAttribPointer(attrib_vPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                check_gl_error();
            }

            if (attrib_texcoord >= 0) {
                glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
                check_gl_error();
                glEnableVertexAttribArray(attrib_texcoord);
                check_gl_error();
                glVertexAttribPointer(attrib_texcoord, 2, GL_FLOAT, GL_FALSE, 0, NULL);
                check_gl_error();
            }


            if (draw_mesh) {
                glDrawElements(GL_TRIANGLES, 0, m_vertex_indices.size(), 0 );
                check_gl_error();
            }
            else {
                int num_patch_vertices = draw_triangle_patches ? 3 : 4;
                glPatchParameteri(GL_PATCH_VERTICES, num_patch_vertices);
                check_gl_error();
                glDrawArrays(GL_PATCHES, 0, 2 * num_patch_vertices);
                check_gl_error();
            }
             */

            glm::mat4 model;
            model = glm::translate( model, m_position);
            glm::mat4 mvp = viewProjection * model;
            float *p = (float *) &mvp[0][0];
            m_shader->setUniformMatrix4fv("ModelViewProjection", 1, p, false);
            m_shader->setUniform1f("Time", (float) 0.0f);

            // Set the texture map
            GLuint iTexture = 0;
            glActiveTexture(GL_TEXTURE0 + iTexture);
            m_material->texture()->bind();
            m_shader->setUniform1i("Texture", iTexture);

            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glEnableVertexAttribArray(m_vcoord_attrib);
            glVertexAttribPointer(m_vcoord_attrib, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                                  0/*stride*/,
                                  0/*offset*/);

            glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoord);
            glEnableVertexAttribArray(m_tcoord_attrib);
            glVertexAttribPointer(m_tcoord_attrib, 2/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                                  0/*stride*/,
                                  0/*offset*/);

            //glDrawArrays(GL_QUADS, 0, 4);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) nullptr);
        }
    }
    glBindVertexArray(0);
}

