//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifdef _WIN32
#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#elif __APPLE__
#define GL3_PROTOTYPES
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <glfw/glfw3.h>
#endif
#include "GL_error.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Texture.h"
#include "utils.h"
#include "Engine.h"
#include "component/KeyboardInput.h"
#include "PointLight.h"
#include "component/Picker.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <tinydir.h>
#include <algorithm>
#include <time.h>

#include "Mesh.h"

using namespace omen;

#define BUFFER_OFFSET(offset)  (0 + offset)

static Shader *normalShader = nullptr;

std::map<std::string, Shader *> Mesh::shaders;
const std::string DEFAULT_TEXTURE = "textures/skull.png";

/**
 * Mesh CTOR
 */
Mesh::Mesh(const std::string &shader,
           Material *material,
           std::vector<glm::vec3> &vertices,
           std::vector<glm::vec3> &normals,
           std::vector<glm::vec2> &texcoords,
           std::vector<GLsizei> indices) {

    initialize();

    glm::vec3 min(0, 0, 0);
    glm::vec3 max(0, 0, 0);
    std::for_each(vertices.begin(), vertices.end(), [&min, &max](glm::vec3 &vec) {
        min.x = std::min(vec.x, min.x);
        min.y = std::min(vec.y, min.y);
        min.z = std::min(vec.z, min.z);
        max.x = std::max(vec.x, max.x);
        max.y = std::max(vec.y, max.y);
        max.z = std::max(vec.z, max.z);
    });
    m_boundingBox.set(min, max);
    create(shader,
           material,
           vertices,
           normals,
           texcoords,
           indices);
}

void Mesh::renderBB() {
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vbo = 0;
    GLuint ibo = 0;
    glGenBuffers(1, &vbo);
    std::vector<glm::vec4> vertices = {{m_boundingBox.Min().x, m_boundingBox.Min().y, m_boundingBox.Min().z, 1.0},
                                       {m_boundingBox.Max().x, m_boundingBox.Min().y, m_boundingBox.Min().z, 1.0},
                                       {m_boundingBox.Max().x, m_boundingBox.Max().y, m_boundingBox.Min().z, 1.0},
                                       {m_boundingBox.Min().x, m_boundingBox.Max().y, m_boundingBox.Min().z, 1.0},
                                       {m_boundingBox.Min().x, m_boundingBox.Min().y, m_boundingBox.Max().z, 1.0},
                                       {m_boundingBox.Max().x, m_boundingBox.Min().y, m_boundingBox.Max().z, 1.0},
                                       {m_boundingBox.Max().x, m_boundingBox.Max().y, m_boundingBox.Max().z, 1.0},
                                       {m_boundingBox.Min().x, m_boundingBox.Max().y, m_boundingBox.Max().z, 1.0}
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * 4 * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    std::vector<GLushort> indices = {0, 1, 2, 3, 4, 5, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

    normalShader->use();
    normalShader->setUniformMatrix4fv("Model", 1, (GLfloat *) &m_transform.tr()[0][0], false);
    normalShader->setUniformMatrix4fv("ModelViewProjection", 1, &Engine::instance()->camera()->viewProjection()[0][0],
                                      false);

    glLineWidth(5.0f);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
            0,  // attribute
            4,                  // number of elements per vertex, here (x,y,z,w)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
    );

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glLineWidth(2.0);
    glPolygonOffset(1, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid *) (4 * sizeof(GLushort)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid *) (8 * sizeof(GLushort)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    //glPolygonOffset(0, 0);

    glDisable(GL_POLYGON_OFFSET_FILL);
    /*glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/
}

void Mesh::create(const std::string &shader,
                  Material *material,
                  std::vector<glm::vec3> &vertices,
                  std::vector<glm::vec3> &normals,
                  std::vector<glm::vec2> &texcoords,
                  std::vector<GLsizei> indices) {
    std::cout << "const std::string &shader,\n"
            "                  Material *material,\n"
            "                  std::vector<glm::vec3> &vertices,\n"
            "                  std::vector<glm::vec3> &normals,\n"
            "                  std::vector<glm::vec2> &texcoords,\n"
            "                  std::vector<GLsizei> indices" << std::endl;
    createShader(shader);
    m_vertex_position_attrib = m_shader->getAttribLocation("position");
    m_vertex_normals_attrib = m_shader->getAttribLocation("normal");
    m_vertex_texture_coord_attrib = m_shader->getAttribLocation("texcoord");


    glm::vec3 min(0, 0, 0);
    glm::vec3 max(0, 0, 0);
    std::for_each(vertices.begin(), vertices.end(), [&min, &max](glm::vec3 &vec) {
        min.x = std::min(vec.x, min.x);
        min.y = std::min(vec.y, min.y);
        min.z = std::min(vec.z, min.z);
        max.x = std::max(vec.x, max.x);
        max.y = std::max(vec.y, max.y);
        max.z = std::max(vec.z, max.z);
    });
    m_boundingBox.set(min, max);

    setMaterial(material);
    if (m_material != nullptr) {
        //if(m_material->texture() == nullptr)
        //    m_material->setTexture(new Texture(getDefaultTexture()));
        m_shader->setMaterial(m_material);
    }

    std::cout << "Size of vertices: " << vertices.size() << std::endl;
    std::cout << "Size of normals: " << normals.size() << std::endl;
    std::cout << "Size of texcoord: " << texcoords.size() << std::endl;

    m_frames.push_back(Frame());
    for (auto v : vertices) m_frames[0].m_vertices.push_back(v);
    for (auto n : normals) m_frames[0].m_normals.push_back(n);
    for (auto t : texcoords) m_texture_coords.push_back(t);
    for (auto i : indices) m_vertex_indices.push_back(i);

    genBuffers();

    if (normalShader == nullptr) {
        normalShader = new Shader("shaders/wireframe.glsl");
        //normalShader = new Shader("shaders/normal_visualizer.glsl");
    }
}

Mesh::Mesh(const std::string &shader, Material *material, std::vector<Mesh::Frame> &frames,
           std::vector<glm::vec2> &texcoords, std::vector<GLsizei> &indices) {
    bRenderBB = true;
    std::cout <<
    "Mesh::Mesh(onst std::string &shader, Material *material, std::vector<Mesh::Frame> &frames,std::vector<glm::vec2> &texcoords, std::vector<GLsizei> &indices)" <<
    std::endl;
    createShader(shader);
    m_vertex_position_attrib = m_shader->getAttribLocation("position");
    m_vertex_normals_attrib = m_shader->getAttribLocation("normal");
    m_vertex_texture_coord_attrib = m_shader->getAttribLocation("texcoord");


    setMaterial(material);
    if (m_material != nullptr) {
        //if(m_material->texture() == nullptr)
        //    m_material->setTexture(new Texture(getDefaultTexture()));
        m_shader->setMaterial(m_material);
    }

    for (auto t : texcoords) m_texture_coords.push_back(t);
    for (auto i : indices) m_vertex_indices.push_back(i);

    for (auto frame : frames) {
        m_frames.push_back(Frame());

        glm::vec3 min(0, 0, 0);
        glm::vec3 max(0, 0, 0);
        std::for_each(frame.m_vertices.begin(), frame.m_vertices.end(), [&min, &max](glm::vec3 &vec) {
            min.x = std::min(vec.x, min.x);
            min.y = std::min(vec.y, min.y);
            min.z = std::min(vec.z, min.z);
            max.x = std::max(vec.x, max.x);
            max.y = std::max(vec.y, max.y);
            max.z = std::max(vec.z, max.z);
        });
        m_boundingBox.set(min, max);


        for (auto v : frame.m_vertices) m_frames.back().m_vertices.push_back(v);
        for (auto n : frame.m_normals) m_frames.back().m_normals.push_back(n);
    }

    genBuffers();
    if (normalShader == nullptr) {
        normalShader = new Shader("shaders/wireframe.glsl");
        //normalShader = new Shader("shaders/normal_visualizer.glsl");
    }

    Picker* picker = Engine::instance()->findComponent<Picker>();
    if(picker)
        picker->signal_object_picked.connect([&](omen::Mesh* obj){
           if(obj==this)
               bRenderBB=true;
            else
               bRenderBB=false;
        });
}


/**
 * Default CTOR with test content
 */
Mesh::Mesh() {
    std::cout << "Mesh::Mesh()" << std::endl;
    Material *m = new Material();
    m->setTexture(new Texture(getDefaultTexture()));

    std::string shader_name = "shaders/pass_through.glsl";
    create(shader_name, m, m_frames[0].m_vertices, m_frames[0].m_normals, m_texture_coords, m_vertex_indices);
}

void Mesh::genBuffers() {
    /**/
    glGenVertexArrays(1, &m_vao);
    check_gl_error();
    glBindVertexArray(m_vao);
    check_gl_error();

    /**
     * Setup the vertex coordinate buffer object (vbo)
     */

    // Enable vertex attributes
    if (m_frames[0].m_vertices.empty()) {
        GLfloat s = 1;
        float aspect = m_material->texture() == nullptr ? 1.0f : (float) m_material->texture()->width() /
                                                                 (float) material()->texture()->height();
        m_frames.push_back(Frame());
        m_frames[0].m_vertices = {
                {-s * aspect, 2, -s},
                {s * aspect,  2, -s},
                {s * aspect,  2, s},
                {-s * aspect, 2, s}};
        m_frames[0].m_normals = {
                {0, 1, 0},
                {0, 1, 0},
                {0, 1, 0},
                {0, 1, 0}};
        m_texture_coords = {
                {0, 0},
                {1, 0},
                {1, 1},
                {0, 1}};
        m_vertex_indices = {0, 1, 3, 1, 2, 3};

    }

    for (auto &frame : m_frames) {
        frame.m_vbo = createVertexCoordBuffer(m_vertex_position_attrib, frame.m_vertices);
        check_gl_error();
        frame.m_vbo_normals = createVertexNormalBuffer(m_vertex_normals_attrib, frame.m_normals);
        check_gl_error();
    }

    createTextureCoordBuffer(m_vertex_texture_coord_attrib, m_texture_coords);
    check_gl_error();
    createIndexBuffer(m_vertex_indices);
    check_gl_error();
}

/**
 * Initialize the shader
 */
void Mesh::initialize() {
    m_vao = 0;
    m_vbo_texcoords = 0;
    m_ibo = 0;

    m_vertex_position_attrib = -1;
    m_vertex_normals_attrib = -1;
    m_vertex_texture_coord_attrib = -1;

    fInnerTess = 1.0;
    fOuterTess = 1.0;
    mPolygonMode = GL_LINE;
    mPolygonMode = GL_FILL;
    m_shader = nullptr;
    m_material = nullptr;
    m_transform.pos() = glm::vec3(0);
    m_amplitude = omen::random(-10, 10) * 0.05;
    m_phase = 3.14 * omen::random(0, 100) / 100.0;
    m_frequency = 0.5 + omen::random(0, 100) / 100.0;

    Engine::instance()->findComponent<KeyboardInput>()->signal_key_release.connect(
            [&](int key, int scanCode, int action, int mods) {
                if (key == GLFW_KEY_O) {
                    mPolygonMode = mPolygonMode == GL_FILL ? GL_LINE : GL_FILL;
                }
            });
}

/**
 * Create a shader for the mesh
 */
void Mesh::createShader(const std::string &shader_name) {
    if (shaders.find(shader_name) == shaders.end()) {
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
    texture_file = files.empty() ? DEFAULT_TEXTURE : files[omen::random(0, files.size() - 1)];
    return texture_file;
}

/**
 * Create vertex index buffer
 */
void Mesh::createIndexBuffer(std::vector<GLsizei> &indices) {
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/**
 * Create a vertex coordinate buffer
 */
GLuint Mesh::createVertexCoordBuffer(GLint vcoord_attrib, std::vector<glm::vec3> &vertices) {
    // Create vbo
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(vcoord_attrib, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                          0/*stride*/, 0/*offset*/);
    return vbo;
}

/**
* Create a vertex normal buffer
*/
GLuint Mesh::createVertexNormalBuffer(GLint vnormal_attrib, std::vector<glm::vec3> &normals) {
    // Create vbo
    GLuint vbo_normals;
    glGenBuffers(1, &vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(m_vertex_normals_attrib, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
                          0/*stride*/, 0/*offset*/);
    return vbo_normals;
}

/**
 * Create a vertex texture coorinate buffer
 */
void Mesh::createTextureCoordBuffer(GLint texcoord_attrib, std::vector<glm::vec2> &texcoords) {
    if (texcoord_attrib >= 0) {
        check_gl_error();

        glGenBuffers(1, &m_vbo_texcoords);
        check_gl_error();
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoords);
        check_gl_error();
        glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(glm::vec2), &texcoords[0], GL_STATIC_DRAW);
        glVertexAttribPointer(m_vertex_texture_coord_attrib, 2/*num elems*/, GL_FLOAT/*elem type*/,
                              GL_FALSE/*normalized*/,
                              0/*stride*/,
                              0/*offset*/);
        check_gl_error();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        check_gl_error();
    }
}


Mesh::~Mesh() {
    std::cout << "Default Mesh destructor" << std::endl;
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

void Mesh::render(const glm::mat4 &viewProjection, const glm::mat4 &view) {
    //m_transform.rotate(Engine::instance()->time(), glm::vec3(0,1,0));
    Engine *e = Engine::instance();
    check_gl_error();
    if (m_shader == nullptr)
        return;

    mPolygonMode = GL_FILL;
    glPolygonOffset(GL_POLYGON_OFFSET_LINE, 0.0);
    render(m_shader, viewProjection, view);

    mPolygonMode = GL_LINE;
    glPolygonOffset(GL_POLYGON_OFFSET_LINE, 0.1);
    glPolygonOffset(GL_POLYGON_OFFSET_FILL, 0.1);
    render(normalShader, viewProjection, view);

    /*
    if (bRenderBB)
        renderBB();
*/
}

void Mesh::render(Shader *shader, const glm::mat4 &viewProjection, const glm::mat4 &view) {
    shader->use();
    glEnable( GL_TEXTURE_2D);
    check_gl_error();
    {
        ////
        //// Tessellation parameters
        ////
        shader->setUniform1f("innerTess", fInnerTess);
        shader->setUniform1f("outerTess", fOuterTess);
        check_gl_error();

        ////
        //// Drawing the mesh
        ////
        {
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
            Engine *e = Engine::instance();
            double t = e->time() * 24.0f;
            glBindVertexArray(m_vao);
            int frame = static_cast<int>(t) % m_frames.size();



            if (false && m_frames.size() > 10) {
                float radius = 3;
                float angle = (3.14f / 180.f) * e->time() * 24.0f;
                m_transform.pos() = {sin(angle) * radius,
                                     m_transform.pos().y,
                                     cos(angle) * radius};

                m_transform.rotate(angle + 1.57f, glm::vec3(0, 1, 0));
            }


            /**
             *
             */
            glm::vec3 lightInvDir = glm::vec3(0.5f, 2, 2);

            // Compute the MVP matrix from the light's point of view
            glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
            glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
            glm::mat4 depthModelMatrix = glm::mat4(1.0);
            glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

            glm::mat4 biasMatrix(
                    0.5, 0.0, 0.0, 0.0,
                    0.0, 0.5, 0.0, 0.0,
                    0.0, 0.0, 0.5, 0.0,
                    0.5, 0.5, 0.5, 1.0
            );
            glm::mat4 depthBiasMVP = biasMatrix * depthMVP;

            // Send our transformation to the currently bound shader,
            // in the "MVP" uniform
            shader->setUniformMatrix4fv("DepthMVP", 1, &depthMVP[0][0], false);

            /**
             *
             */
            glm::mat4 model = m_transform.tr();
            glm::mat4 mvp = viewProjection;
            glm::mat4 mv = view;
            glm::mat4 mvi = glm::transpose(glm::inverse(mv));
            glm::mat4 mi = glm::transpose(glm::inverse(model));

            shader->setUniformMatrix4fv("Model", 1, (GLfloat *) &model[0], false);
            shader->setUniformMatrix4fv("View", 1, (GLfloat *) &view[0], false);
            shader->setUniformMatrix4fv("ModelViewProjection", 1, (GLfloat *) &mvp[0], false);
            shader->setUniformMatrix4fv("ModelView", 1, (GLfloat *) &mv[0], false);
            shader->setUniformMatrix4fv("ModelViewInverse", 1, (GLfloat *) &mvi[0], false);
            shader->setUniformMatrix4fv("NormalMatrix", 1, (GLfloat *) &mi[0], false);
            shader->setUniform1f("Time", (float) 0.0f);

            // Light
            std::vector<std::shared_ptr<Light> > &lights = Engine::instance()->scene()->lights();
            for (auto light : lights) {
                if(dynamic_cast<PointLight*>(light.get())) {
                    PointLight* plight = static_cast<PointLight*>(light.get());
                    glm::vec3 lpos = plight->tr().pos();
                    glm::vec3 lcolor = plight->color();
                    shader->setUniform3fv("LightPosition",1, (GLfloat*) &lpos);
                    shader->setUniform3fv("LightColor", 1, (GLfloat*) &lcolor);
                    shader->setUniform1f("LightIntensity", plight->intensity());
                }
            }

            /*
            //shader->setUniformMatrix4fv("View", 1, (GLfloat *)&Engine::instance()->camera()->view()[0][0], false);
            shader->setUniformMatrix4fv("Model", 1, (GLfloat *) &glm::mat4(1)[0][0], false);
            shader->setUniformMatrix4fv("ModelViewProjection", 1, &Engine::instance()->camera()->viewProjection()[0][0], false);
            //shader->setUniformMatrix4fv("ModelView", 1, (GLfloat *) &mv[0], false);
            //shader->setUniformMatrix4fv("ModelViewInverse", 1, (GLfloat *) &mvi[0], false);
            //shader->setUniformMatrix4fv("NormalMatrix", 1, (GLfloat *) &mi[0], false);
            //shader->setUniform1f("Time", (float) 0.0f);
            */

            /*
            if (m_material->twoSided()) {
                //glDisable(GL_CULL_FACE);
                //glDisable(GL_DEPTH_TEST);
                glEnable (GL_BLEND);
                glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
            }
            else {
                glEnable(GL_CULL_FACE);
                //glEnable(GL_DEPTH_TEST);
            }
            */

            shader->setUniform4fv("DiffuseColor", 1, &m_material->diffuseColor()[0]);
            shader->setUniform4fv("AmbientColor", 1, &m_material->ambientColor()[0]);
            shader->setUniform4fv("SpecularColor", 1, &m_material->specularColor()[0]);

            if (m_frames.size() > frame) {
                glBindBuffer(GL_ARRAY_BUFFER, m_frames[frame].m_vbo);
                glVertexAttribPointer(m_vertex_position_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
                glBindBuffer(GL_ARRAY_BUFFER, m_frames[frame].m_vbo_normals);
                glVertexAttribPointer(m_vertex_normals_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
            }

            if (m_material->texture()) {
                shader->setUniform1i("TextureEnabled", 1);
                glEnable(GL_TEXTURE_2D);
                glActiveTexture(GL_TEXTURE0);
                //glBindTexture(GL_TEXTURE_2D, m_material->texture()->id());
                m_material->texture()->bind();
                //shader->setUniform1i("Texture",);
            }
            else{
                shader->setUniform1i("TextureEnabled", 0);
            }
            if (m_material->matcapTexture()) {
                glEnable(GL_TEXTURE_2D);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_material->matcapTexture()->id());
                shader->setUniform1i("MetacapTextureEnabled", true);
            }
            else {
                shader->setUniform1i("MetacapTextureEnabled", false);
            }

            shader->setUniform1i("TextureEnabled", 1);
            glEnableVertexAttribArray(m_vertex_position_attrib);
            glEnableVertexAttribArray(m_vertex_normals_attrib);
            glEnableVertexAttribArray(m_vertex_texture_coord_attrib);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
            glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode);
            glDrawElements(GL_TRIANGLES, m_vertex_indices.size(), GL_UNSIGNED_INT, (void *) nullptr);
        }
    }
    glBindVertexArray(0);
}

omen::BoundingBox &Mesh::aabb() {
    m_boundingBox.tr() = m_transform;
    return m_boundingBox;
}
