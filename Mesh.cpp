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
#include "utils.h"
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

std::map<std::string, Shader*> Mesh::shaders;
const std::string DEFAULT_TEXTURE = "textures/skull.png";

/**
 * Mesh CTOR
 */
Mesh::Mesh(std::vector<glm::vec3> &vertices,
           std::vector<glm::vec3> &normals,
           std::vector<glm::vec2> &texcoords,
           std::vector<GLsizei> indices) {

    initialize();

    m_vertices = vertices;
    m_normals = normals;
    m_texture_coords = texcoords;
    m_vertex_indices = indices;
}

/**
 * Default CTOR with test content
 */
Mesh::Mesh() {
    initialize();

    setMaterial(new Material);
    m_material->setTexture(new Texture(getDefaultTexture()));

    std::string shader_name = "shaders/pass_through.glsl";
    createShader(shader_name);

    if (m_material != nullptr)
        m_shader->setMaterial(m_material);

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
        float aspect = (float)m_material->texture()->width()/(float)material()->texture()->height();
        m_vertices =  {
                {-s*aspect, 2, -s},
                {s*aspect,  2, -s},
                {s*aspect,  2, s},
                {-s*aspect, 2, s}};
        createVertexCoordBuffer(m_vcoord_attrib, m_vertices);
        check_gl_error();

        /**
         * Setup the vertex index element buffer (ibo)
         */
        m_vertex_indices = {0, 1, 3, 1, 2, 3};
        createIndexBuffer(m_vertex_indices);
        check_gl_error();
    }

    /**
     * Setup the vertex normals
     */
    m_vertex_normals_attrib = m_shader->getAttribLocation("normal");
    if(m_vertex_normals_attrib>=0){
        m_normals =  {
                {0,1,0},
                {0,1,0},
                {0,1,0},
                {0,1,0}};
        createVertexNormalBuffer(m_vertex_normals_attrib, m_normals);
        check_gl_error();
    }

    /**
     * Setup the vertex texture coordinates
     */
    m_tcoord_attrib = m_shader->getAttribLocation("texcoord");
    std::vector<glm::vec2> texcoords = {
            {0, 0},
            {1, 0},
            {1, 1},
            {0, 1}
    };
    createTextureCoordBuffer(m_tcoord_attrib, texcoords);
    check_gl_error();

    /**/
}

/**
 * Initialize the shader
 */
void Mesh::initialize()
{
    fInnerTess = 1.0;
    fOuterTess = 1.0;
    mPolygonMode = GL_LINE;
    m_shader = nullptr;
    m_material = nullptr;
    m_position = glm::vec3(0);
    m_amplitude = Omen::random(-10,10)*0.05;
    m_phase = 3.14*Omen::random(0,100)/100.0;
    m_frequency = 0.5+Omen::random(0,100)/100.0;
}

/**
 * Create a shader for the mesh
 */
void Mesh::createShader(const std::string& shader_name)
{
    if(shaders.find(shader_name) == shaders.end()){
        m_shader = new Shader(shader_name);
        shaders[shader_name] = m_shader;
    }
    else
        m_shader = shaders[shader_name];
}

/**
 * Returns a default texture file-path
 */
std::string Mesh::getDefaultTexture() {
    std::string texture_file = DEFAULT_TEXTURE;

    static std::vector<std::string> files;
    tinydir_dir dir;
    std::string filename;
    std::vector<std::string> accepted_exts = {".jpg", ".png"};

    /*if(files.empty())
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
*/
    texture_file = files.empty() ? DEFAULT_TEXTURE : files[Omen::random(0,files.size()-1)];
    return texture_file;
}

/**
 * Create vertex index buffer
 */
void Mesh::createIndexBuffer(std::vector<GLsizei> &indices)
{
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/**
 * Create a vertex coordinate buffer
 */
void Mesh::createVertexCoordBuffer(GLint vcoord_attrib, std::vector<glm::vec3>& vertices)
{
    // Create vbo
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(m_vcoord_attrib, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                          0/*stride*/, 0/*offset*/);
}

/**
 * Create a vertex texture coorinate buffer
 */
void Mesh::createTextureCoordBuffer(GLint texcoord_attrib, std::vector<glm::vec2> &texcoords)
{
    if (texcoord_attrib >= 0) {
        check_gl_error();

        glGenBuffers(1, &m_vbo_texcoords);
        check_gl_error();
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoords);
        check_gl_error();
        glBufferData(GL_ARRAY_BUFFER, texcoords.size()*sizeof(glm::vec2), &texcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(m_tcoord_attrib, 2/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                              0/*stride*/,
                              0/*offset*/);
        check_gl_error();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        check_gl_error();
    }
}



/**
 * Create a vertex normal buffer
 */
void Mesh::createVertexNormalBuffer(GLint vnormal_attrib, std::vector<glm::vec3>& normals)
{
    // Create vbo
    glGenBuffers(1, &m_vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(m_vertex_normals_attrib, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                          0/*stride*/, 0/*offset*/);
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

    /*
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

            glEnableVertexAttribArray(m_vcoord_attrib);
            glEnableVertexAttribArray(m_vertex_normals_attrib);
            glEnableVertexAttribArray(m_tcoord_attrib);

            //glDrawArrays(GL_QUADS, 0, 4);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
            glDrawElements(GL_TRIANGLES, m_vertex_indices.size(), GL_UNSIGNED_INT, (void *) nullptr);
        }
    }
    glBindVertexArray(0);
}


