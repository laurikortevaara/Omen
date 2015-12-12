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

#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>

#define BUFFER_OFFSET(offset)  (0 + offset)

GLuint vao = 0;
GLuint vbo = 0;
GLuint vbo_barycentric = 0;
GLuint vbo_texcoord = 0;
GLint attrib_vPosition;
GLint attrib_barycentric;
GLint attrib_texcoord;

GLuint shader_program = 0;
GLuint textureID = 0;
GLboolean draw_mesh = 1;
GLboolean draw_triangle_patches = 1;

void loadShader(const std::string &filename, std::string &out) {
    std::string line;
    std::ifstream in(filename.c_str());
    while (std::getline(in, line)) {
        out += line + "\n";
    }
}



Mesh::Mesh() : fInnerTess(1.0), fOuterTess(1.0), mPolygonMode(GL_FILL) {
    loadShaders();
    check_gl_error();
    loadTextures();
    check_gl_error();
    createPatches();
}

Mesh::~Mesh() {

}


void Mesh::loadTextures() {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int x, y, btm;
    FILE *f = fopen("heightmap.jpg", "r");
    stbi_uc *image = stbi_load_from_file(f, &x, &y, &btm, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexImage2D(GL_TEXTURE_2D, 0, btm == 3 ? GL_RGB : GL_RGBA, x, y, 0, btm == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE,
                 (const void *) image);
    check_gl_error();
    stbi_image_free(image);
}

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
    // Create the Vertex Array
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Create the Vertex Buffer Object
    std::vector<std::tuple<GLfloat, GLfloat, GLfloat>> vertices = {{-1,0,0},{0,1,0},{1,0,0}};
    // An array of 3 vectors which represents 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f,  1.0f, 0.0f,
            };
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), &g_vertex_buffer_data[0], GL_STATIC_DRAW );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 0, (void*)0);

    // Create the vertex index buffer
    std::vector<unsigned int> indices = {0,1,2};
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);

}


void Mesh::setupModelView()
{
    ////
    //// Projection Matrix
    ////
    GLint mvp_handle = glGetUniformLocation(shader_program, "ModelViewProjectionMatrix");
    if (mvp_handle >= 0) {
        double ViewPortParams[4];
        glGetDoublev(GL_VIEWPORT, ViewPortParams);

        // Generates a really hard-to-read matrix, but a normal, standard 4x4 matrix nonetheless
        glm::mat4 Projection = glm::perspective(
                90.0f,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
                4.0f /
                3.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
                0.1f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
                100.0f       // Far clipping plane. Keep as little as possible.
        );
        // Camera matrix
        glm::mat4 View = glm::lookAt(
                glm::vec3(0, 0, 1), // Camera is at (4,3,3), in World Space
                glm::vec3(0, 0, 0), // and looks at the origin
                glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
        static float angle = 0.0f;
        View = glm::rotate(View,  angle, glm::vec3(0.0f,1.0f,0.0f));
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 Model = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

        glUniformMatrix4fv(mvp_handle, 1, GL_FALSE, &mvp[0][0]);
        check_gl_error();
    }

}

void Mesh::render() {
    glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode);
    glUseProgram(shader_program);


    setupModelView();

    glBindVertexArray(m_vao);
    //glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
}

