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
GLboolean draw_mesh = 0;
GLboolean draw_triangle_patches = 1;

GLfloat vertices_quad[][3] = {
        {-1, -1, 0},
        {-1, 1,  0},
        {1,  1,  0},
        {1,  -1, 0}// vPosition
};

GLfloat vertices_tri[][3] = {
        {-1, -1, 0},
        {-1,  1, 0},
        { 1, -1, 0}, // vPosition
        {-1,  1, 0},
        { 1,  1, 0},
        { 1, -1, 0}
};

GLfloat uvCoords_quad[][2] = {
        {0, 0},
        {0, 1},
        {1, 1},
        {1, 0}};

GLfloat uvCoords_tri[][2] = {
        {0, 0},
        {0, 1},
        {1, 0},
        {0, 1},
        {1, 1},
        {1, 0},};

GLfloat baryCoords_quad[][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
        {1, 0, 0}
};

GLfloat baryCoord_tri[][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
};


void loadShader(const std::string &filename, std::string &out) {
    std::string line;
    std::ifstream in(filename.c_str());
    while (std::getline(in, line)) {
        out += line + "\n";
    }
}



Mesh::Mesh() : fInnerTess(1.0), fOuterTess(1.0), mPolygonMode(GL_LINE) {
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
    FILE *f = fopen("checker.jpg", "r");
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


void Mesh::render() {
    glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode);
    glUseProgram(shader_program);

    check_gl_error();
    {
        ////
        //// Tessellation parameters
        ////
        GLint tessLevelInner = glGetUniformLocation(shader_program, "innerTess");
        check_gl_error();

        GLint tessLevelOuter = glGetUniformLocation(shader_program, "outerTess");
        check_gl_error();

        glUniform1f(tessLevelInner, fInnerTess);
        glUniform1f(tessLevelOuter, fOuterTess);
        check_gl_error();

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
                    glm::vec3(1, 0, 1), // Camera is at (4,3,3), in World Space
                    glm::vec3(0, 0, 0), // and looks at the origin
                    glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
            );
            // Model matrix : an identity matrix (model will be at the origin)
            glm::mat4 Model = glm::mat4(1.0f);
            // Our ModelViewProjection : multiplication of our 3 matrices
            glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

            glUniformMatrix4fv(mvp_handle, 1, GL_FALSE, &mvp[0][0]);
            check_gl_error();
        }

        ////
        //// Drawing the mesh
        ////
        {

            GLint useTextureUniform = glGetUniformLocation(shader_program, "useTexture");
            glUniform1i(useTextureUniform, m_use_texture ? 1 : 0 );

            ////
            //// Texture
            ////
            GLint texUniform = glGetUniformLocation(shader_program, "tex");
            check_gl_error();

            glActiveTexture(GL_TEXTURE0);
            check_gl_error();
            glBindTexture(GL_TEXTURE_2D, textureID);
            check_gl_error();
            glUniform1i(texUniform, /*GL_TEXTURE*/0);
            check_gl_error();


            glBindVertexArray(vao);
            check_gl_error();

            // Render
            if (attrib_barycentric >= 0) {
                glBindBuffer(GL_ARRAY_BUFFER, vbo_barycentric);
                check_gl_error();
                glEnableVertexAttribArray(attrib_barycentric);
                check_gl_error();
                glVertexAttribPointer(attrib_barycentric, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                check_gl_error();
            }
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
                glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
                check_gl_error();
            }
            else {
                int num_patch_vertices = draw_triangle_patches ? 3 : 4;
                glPatchParameteri(GL_PATCH_VERTICES, num_patch_vertices);
                check_gl_error();
                glDrawArrays(GL_PATCHES, 0, 2*num_patch_vertices);
                check_gl_error();
            }
        }
    }
}

