//
// Created by Lauri Kortevaara(personal) on 21/12/15.
//

#ifndef OMEN_SHADER_H
#define OMEN_SHADER_H

#include <memory>

#include "Texture.h"
#include "Material.h"

namespace omen {
    class Shader {
    public:
        Shader(const std::string &shader_file);

        void use() const;

        bool readShaderFile(const std::string &shader_file);

        GLuint m_shader_program;

        void use();

        GLint getAttribLocation(const std::string &attribName);

        GLint getUniformLocation(const std::string &uniformName);

        void setUniform1i(const std::string &uniform, GLint i);

        void setUniform1d(const std::string &uniform, double value);

        void setUniform1f(const std::string &uniform, float value);

        void setUniform1dv(const std::string &uniform, int count, double *value);

        void setUniform1fv(const std::string &uniform, int count, float *value);

        void setUniform1iv(const std::string &uniform, int count, int *value);

        void setUniform1ui(const std::string &uniform, unsigned int value);

        void setUniform1uiv(const std::string &uniform, int count, unsigned int *value);

        void setUniform2d(const std::string &uniform, double x, double y);

        void setUniform2dv(const std::string &uniform, int count, double *values);

        void setUniform2i(const std::string &uniform, int x, int y);

        void setUniform2iv(const std::string &uniform, int count, int *values);

        void setUniform2ui(const std::string &uniform, unsigned int x, unsigned int y);

        void setUniform2uiv(const std::string &uniform, int count, unsigned int *values);

        void setUniform2f(const std::string &uniform, float x, float y);

        void setUniform2fv(const std::string &uniform, int count, float *values);

        void setUniform3i(const std::string &uniform, int x, int y, int z);

        void setUniform3iv(const std::string &uniform, int count, int *values);

        void setUniform3ui(const std::string &uniform, unsigned int x, unsigned int y, unsigned int z);

        void setUniform3uiv(const std::string &uniform, int count, unsigned int *values);

        void setUniform3f(const std::string &uniform, float x, float y, float z);

        void setUniform3fv(const std::string &uniform, int count, float *values);

        void setUniform3d(const std::string &uniform, double x, double y, double z);

        void setUniform3dv(const std::string &uniform, int count, double *values);

        void setUniform4i(const std::string &uniform, int x, int y, int z, int w);

        void setUniform4iv(const std::string &uniform, int count, int *values);

        void setUniform4ui(const std::string &uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w);

        void setUniform4uiv(const std::string &uniform, int count, unsigned int *values);

        void setUniform4f(const std::string &uniform, float x, float y, float z, float w);

        void setUniform4fv(const std::string &uniform, int count, float *values);

        void setUniform4d(const std::string &uniform, double x, double y, double z, double w);

        void setUniform4dv(const std::string &uniform, int count, double *values);

        void setUniformBlockBinding(const std::string &uniform_block, int block_binding);

        void setUniformMatrix2fv(const std::string &uniform, int count, float *values, bool bTranspose);

        void setUniformMatrix2dv(const std::string &uniform, int count, double *values, bool bTranspose);

        void setUniformMatrix3fv(const std::string &uniform, int count, float *values, bool bTranspose);

        void setUniformMatrix3dv(const std::string &uniform, int count, double *values, bool bTranspose);

        void setUniformMatrix4fv(const std::string &uniform, int count, float *values, bool bTranspose);

        void setUniformMatrix4dv(const std::string &uniform, int count, double *values, bool bTranspose);


        void setMaterial(std::unique_ptr<Material> material);

        void setTexture(int textureIndex, const std::string& textureName, Texture *texture);

        std::string printShaderSource(GLuint &shader_id);

        std::string getShaderSource(GLuint &shader_id);

        bool createShader(GLenum shaderType, GLuint &shader_id, std::string shader_source);
    };
} // namespace omen

#endif //OMEN_SHADER_H
