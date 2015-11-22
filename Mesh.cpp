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

GLfloat vertices [][3] = {
        {-1, -1, 0}, {-1, 1, 0}, { 1,  1, 0}, { 1, -1, 0}// vPosition
};

GLfloat uvCoords[][2] = {
        {0,0},{0,1},{1,1},{1,0} };

/*
 {-1, 0, -1}, {-1, 1, 1}, { 1, 0, -1}, // vPosition
 { 1, 0, -1}, {-1, 1, 0}, { 1, 0,  1} // vPosition
 */

/*
{-1, -1, 0}, {-1, 1, 0}, { 1, -1, 0}, // vPosition
{ 1, -1, 0}, {-1, 1, 0}, { 1,  1, 0} // vPosition
 */

GLfloat baryCoords [][3] = {
        { 1, 0, 0}, { 0, 1, 0}, { 0,  0, 1 }, { 1, 0, 0},// baryCentric
        { 0, 1, 0}, { 0,  0, 1} // baryCentric
};


void loadShader( const std::string& filename, std::string& out ) {
    std::string line;
    std::ifstream in(filename.c_str());
    while(std::getline(in, line))
    {
        out += line + "\n";
    }
}

void Mesh::loadTextures()
{
    glGenTextures(1,&textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glGenerateMipmap(GL_TEXTURE_2D);

    int x,y,btm;
    FILE* f = fopen("bubble.png","r");
    stbi_uc* image = stbi_load_from_file(f, &x, &y, &btm, 0);
    /*
    x = 256;
    y = 256;
    char * image = (char*)malloc(4*x*y);
    memset(image, 0xff, x*y*4);*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*) image);
    check_gl_error();
    stbi_image_free(image);

    GLuint isTexture = glIsTexture(textureID);
    check_gl_error();
}

void Mesh::loadShaders()
{
    shader_program = glCreateProgram();

    std::string vertex_shader = "";
    std::string control_shader = "";
    std::string eval_shader = "";
    std::string geom_shader = "";
    std::string fragment_shader = "";
    if(draw_mesh)
    {
        loadShader("mesh_shaders/vs.glsl", vertex_shader);
        loadShader("mesh_shaders/fs.glsl", fragment_shader);
    }
    else
    {
        loadShader("patch_shaders/vs.glsl", vertex_shader);
        loadShader("patch_shaders/tc_quad.glsl", control_shader);
        loadShader("patch_shaders/te_quad.glsl", eval_shader);
        //loadShader("patch_shaders/gs.glsl", geom_shader);
        loadShader("patch_shaders/fs.glsl", fragment_shader);
    }


    GLint sizei = 255;
    GLchar log1[255] = {""};
    GLchar log2[255] = {""};
    GLchar log3[255] = {""};
    GLchar log4[255] = {""};
    GLchar log5[255] = {""};
    GLchar log_program[255];

    if(!vertex_shader.empty())
    {
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        const GLchar* ss = vertex_shader.c_str();
        glShaderSource(vs, 1, &ss, NULL);
        glCompileShader(vs);
        glGetShaderInfoLog(vs,255, &sizei, log1);
        assert(strlen(log1)==0);
        glAttachShader(shader_program, vs);
    }

    if(!control_shader.empty()) {
        GLuint tc = glCreateShader(GL_TESS_CONTROL_SHADER);
        const GLchar *ss = control_shader.c_str();
        glShaderSource(tc, 1, &ss, NULL);
        glCompileShader(tc);
        glGetShaderInfoLog(tc, 255, &sizei, log2);
        assert(strlen(log2)==0);
        glAttachShader(shader_program, tc);
        check_gl_error();
    }

    if(!eval_shader.empty()) {
        GLuint te = glCreateShader(GL_TESS_EVALUATION_SHADER);
        const GLchar* ss = eval_shader.c_str();
        glShaderSource(te, 1, &ss, NULL);
        glCompileShader(te);
        glGetShaderInfoLog(te, 255, &sizei, log3);
        assert(strlen(log3)==0);
        glAttachShader(shader_program, te);
    }

    if(!geom_shader.empty()) {

        GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
        const GLchar* ss = geom_shader.c_str();
        glShaderSource(gs, 1, &ss, NULL);
        glCompileShader(gs);
        glGetShaderInfoLog(gs, 255, &sizei, log4);
        assert(strlen(log4)==0);
        glAttachShader(shader_program, gs);
        check_gl_error();
    }

    if(!fragment_shader.empty())
    {
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        const GLchar* ss = fragment_shader.c_str();
        glShaderSource(fs, 1, &ss, NULL);
        glCompileShader(fs);
        glGetShaderInfoLog(fs, 255, &sizei, log5);
        assert(strlen(log5)==0);
        glAttachShader(shader_program, fs);
        check_gl_error();
    }

    glLinkProgram(shader_program);
    glGetProgramInfoLog(shader_program,255, &sizei, log_program);
    check_gl_error();
    assert(strlen(log5)==0);
}

void Mesh::createPatches()
{
    attrib_vPosition  = glGetAttribLocation(shader_program, "vPosition" );
    attrib_barycentric = glGetAttribLocation(shader_program, "baryCentric" );
    attrib_texcoord  = glGetAttribLocation(shader_program, "texcoord" );

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
    if(attrib_vPosition>=0) {
        glGenBuffers(1,&vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    }

    ///
    /// Create Array Buffer for barycentric coordinates
    ///
    if(attrib_barycentric>=0) {
        glGenBuffers(1, &vbo_barycentric);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_barycentric);
        glBufferData(GL_ARRAY_BUFFER, sizeof(baryCoords), baryCoords, GL_STATIC_DRAW);
    }

    ///
    /// Create Array Buffer for texture coordinates
    ///
    if(attrib_texcoord>=0) {
        // Init resources
        glGenBuffers(1, &vbo_texcoord);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uvCoords), uvCoords, GL_STATIC_DRAW);
    }
}

Mesh::Mesh() : fInnerTess(1.0),fOuterTess(1.0)
{
    loadShaders();
    check_gl_error();
    loadTextures();
    check_gl_error();
    //if(draw_mesh)
    //    createMesh();
    //else
        createPatches();
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
        GLint mvp = glGetUniformLocation(shader_program, "ModelViewProjectionMatrix");
        if(mvp>=0)
        {
            double ViewPortParams[4];
            glGetDoublev(GL_VIEWPORT, ViewPortParams);

            static float i = 0.0;
            i += 0.05;
            glm::mat4 mvpi = glm::mat4(1.0);
            //mvpi = glm::translate(mvpi, glm::vec3(0,0,0));
            mvpi = glm::scale(mvpi, glm::vec3(0.5*(720.0/1280.0),0.5,0.5));
            //mvpi = glm::rotate(mvpi, i, glm::vec3(0,0,1));
            check_gl_error();
            glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(mvpi));
            check_gl_error();
        }

        ////
        //// Drawing the mesh
        ////
        {
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
            if(attrib_barycentric>=0) {
                glBindBuffer(GL_ARRAY_BUFFER, vbo_barycentric);
                check_gl_error;
                glEnableVertexAttribArray(attrib_barycentric);
                check_gl_error();
                glVertexAttribPointer(attrib_barycentric, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                check_gl_error();
            }
            if(attrib_vPosition>=0) {
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                check_gl_error();
                glEnableVertexAttribArray(attrib_vPosition);
                check_gl_error();
                glVertexAttribPointer(attrib_vPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);
                check_gl_error();
            }

            if(attrib_texcoord>=0) {
                glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
                check_gl_error();
                glEnableVertexAttribArray(attrib_texcoord);
                check_gl_error();
                glVertexAttribPointer(attrib_texcoord, 2, GL_FLOAT, GL_FALSE, 0, NULL);
                check_gl_error();
            }



            if(draw_mesh)
            {
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle
                check_gl_error();
            }
            else
            {
                glPatchParameteri(GL_PATCH_VERTICES,4);
                check_gl_error();
                glDrawArrays(GL_PATCHES, 0, 4);
                check_gl_error();
            }
        }
    }
}

