//
// Created by Lauri Kortevaara(personal) on 21/12/15.
//

#include <OpenGL/gl3.h>

#include <boxer/boxer.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>

#include "GL_error.h"
#include "Shader.h"


using namespace Omen;

Shader::Shader(const std::string &shader_file) {
    readShaderFile(shader_file);

}

bool Shader::readShaderFile(const std::string& shader_file) {
    std::ifstream is (shader_file, std::ifstream::binary);
    if (is) {
        // get length of file:
        is.seekg (0, is.end);
        int length = is.tellg();
        is.seekg (0, is.beg);

        char * buffer = new char [length];

        std::cout << "Reading " << length << " characters... ";
        // read data as a block:
        is.read (buffer,length);

        if (!is.eof())
            std::cout << "all characters read successfully.";
        else
            std::cout << "error: only " << is.gcount() << " could be read";
        is.close();

        // ...buffer contains the entire file...
        std::string myshader = buffer;
        std::string vsdefines = "#version 410\n#define VERTEX_SHADER\n"+myshader;
        std::string fsdefines = "#version 410\n#define FRAGMENT_SHADER\n"+myshader;

        GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
        check_gl_error();
        GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
        check_gl_error();

        const char* vertex_shader = vsdefines.c_str();
        const char* fragment_shader = fsdefines.c_str();
        glShaderSource(vShader,1, &vertex_shader, nullptr);
        check_gl_error();
        glShaderSource(fShader,1, &fragment_shader, nullptr);
        check_gl_error();
        m_shader_program = glCreateProgram();
        check_gl_error();

        GLchar info_log[1024];

        glCompileShader(vShader);
        check_gl_error();

        int info_len = 0;
        glGetShaderInfoLog(vShader,sizeof(info_log), &info_len, info_log);
        if(strlen(info_log)!=0){
            boxer::show(info_log, "Error1");
            boxer::show(myshader.c_str(), "Vertex Shader");
            exit(0);
        }

        glCompileShader(fShader);
        check_gl_error();

        glGetShaderInfoLog(fShader,sizeof(info_log), &info_len, info_log);
        if(strlen(info_log)!=0){
            boxer::show(info_log, "Error2");
            boxer::show(myshader.c_str(), "Fragment Shader");
            exit(0);
        }

        glAttachShader(m_shader_program, vShader);
        check_gl_error();
        glAttachShader(m_shader_program, fShader);
        check_gl_error();
        glLinkProgram(m_shader_program);

        GLchar vertex_shader_source[1024];
        memset(vertex_shader_source, 0, 1024);
        GLsizei vertex_shader_source_len = 0;
        glGetShaderSource(fShader, sizeof(vertex_shader_source), &vertex_shader_source_len, vertex_shader_source);

        GLchar fragment_shader_source[1024];
        memset(fragment_shader_source, 0, 1024);
        GLsizei fragment_shader_source_len = 0;
        glGetShaderSource(fShader, sizeof(fragment_shader_source), &fragment_shader_source_len, fragment_shader_source);


        glGetProgramInfoLog(m_shader_program,sizeof(info_log), &info_len, info_log);
        if(strlen(info_log)!=0){
            boxer::show(info_log, "Error3");
            //exit(0);
        }


        delete[] buffer;
    }

    return true;
}

void Shader::use() {
    glUseProgram(m_shader_program);
}


/**
 * 1D
 */
void Shader::setUniform1i(const std::string& uniform, GLint value){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform1i(uniform_loc,value);
}

void Shader::setUniform1iv(const std::string& uniform, int count, int* value){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform1iv(uniform_loc, count, value);
}

void Shader::setUniform1ui(const std::string& uniform, unsigned int value){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform1ui(uniform_loc, value);
}

void Shader::setUniform1uiv(const std::string& uniform, int count, unsigned int* value){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform1uiv(uniform_loc, count, value);
}

void Shader::setUniform1f(const std::string& uniform, float value){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform1f(uniform_loc,value);
}

void Shader::setUniform1fv(const std::string& uniform, int count, float* value){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform1fv(uniform_loc, count, value);
}

void Shader::setUniform1d(const std::string& uniform, double value){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform1d(uniform_loc,value);
}

void Shader::setUniform1dv(const std::string& uniform, int count, double* value){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform1dv(uniform_loc, count, value);
}

/**
 * 2D
 */
void Shader::setUniform2i(const std::string& uniform, int x, int y){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform2i(uniform_loc, x, y);
}

void Shader::setUniform2iv(const std::string& uniform, int count, int* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform2iv(uniform_loc, count, values);
}

void Shader::setUniform2ui(const std::string& uniform, unsigned int x, unsigned int y){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform2ui(uniform_loc, x, y);
}

void Shader::setUniform2uiv(const std::string& uniform, int count, unsigned int* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform2uiv(uniform_loc, count, values);
}

void Shader::setUniform2f(const std::string& uniform, float x, float y){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform2f(uniform_loc, x, y);
}

void Shader::setUniform2fv(const std::string& uniform, int count, float* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform2fv(uniform_loc, count, values);
}

void Shader::setUniform2d(const std::string& uniform, double x, double y){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform2d(uniform_loc, x, y);
}

void Shader::setUniform2dv(const std::string& uniform, int count, double* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform2dv(uniform_loc, count, values);
}

/**
 * 3D
 */
void Shader::setUniform3i(const std::string& uniform, int x, int y, int z){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform3i(uniform_loc, x, y, z);
}

void Shader::setUniform3iv(const std::string& uniform, int count, int* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform3iv(uniform_loc, count, values);
}

void Shader::setUniform3ui(const std::string& uniform, unsigned int x, unsigned int y, unsigned int z){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform3ui(uniform_loc, x, y, z);
}

void Shader::setUniform3uiv(const std::string& uniform, int count, unsigned int* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform3uiv(uniform_loc, count, values);
}

void Shader::setUniform3f(const std::string& uniform, float x, float y, float z){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform3f(uniform_loc, x, y, z);
}

void Shader::setUniform3fv(const std::string& uniform, int count, float* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform3fv(uniform_loc, count, values);
}

void Shader::setUniform3d(const std::string& uniform, double x, double y, double z){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform3d(uniform_loc, x, y, z);
}

void Shader::setUniform3dv(const std::string& uniform, int count, double* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform3dv(uniform_loc, count, values);
}


/**
 * 4D
 */
void Shader::setUniform4i(const std::string& uniform, int x, int y, int z, int w){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform4i(uniform_loc, x, y, z, w);
}

void Shader::setUniform4iv(const std::string& uniform, int count, int* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform4iv(uniform_loc, count, values);
}

void Shader::setUniform4ui(const std::string& uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform4ui(uniform_loc, x, y, z, w);
}

void Shader::setUniform4uiv(const std::string& uniform, int count, unsigned int* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform4uiv(uniform_loc, count, values);
}

void Shader::setUniform4f(const std::string& uniform, float x, float y, float z, float w){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform4f(uniform_loc, x, y, z, w);
}

void Shader::setUniform4fv(const std::string& uniform, int count, float* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform4fv(uniform_loc, count, values);
}

void Shader::setUniform4d(const std::string& uniform, double x, double y, double z, double w){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform4d(uniform_loc, x, y, z, w);
}

void Shader::setUniform4dv(const std::string& uniform, int count, double* values){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniform4dv(uniform_loc, count, values);
}


/**
 * Uniform block binding
 */
void Shader::setUniformBlockBinding(const std::string& uniform_block, int block_binding){
    GLint block_index = glGetUniformBlockIndex(m_shader_program,uniform_block.c_str());
    glUniformBlockBinding(m_shader_program, block_index, block_binding);
}


/**
 * Matrix uniforms
 */
void Shader::setUniformMatrix2fv(const std::string& uniform, int count, float* values, bool bTranspose){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniformMatrix2fv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix2dv(const std::string& uniform, int count, double* values, bool bTranspose){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniformMatrix2dv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix3fv(const std::string& uniform, int count, float* values, bool bTranspose){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniformMatrix3fv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix3dv(const std::string& uniform, int count, double* values, bool bTranspose){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniformMatrix3dv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix4fv(const std::string& uniform, int count, float* values, bool bTranspose){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniformMatrix4fv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix4dv(const std::string& uniform, int count, double* values, bool bTranspose){
    GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
    glUniformMatrix4dv(uniform_loc, count, bTranspose, values);
}


GLint Shader::getAttribLocation(const std::string &attribName) {
    return glGetAttribLocation(m_shader_program, attribName.c_str());
}

GLint Shader::getUniformLocation(const std::string &uniformName) {
    return glGetUniformLocation(m_shader_program, uniformName.c_str());
}
