//
// Created by Lauri Kortevaara on 21/12/15.
//

#ifndef OMEN_SHADER_H
#define OMEN_SHADER_H

#include <memory>
#include <vector>
#include <string>

#include "Texture.h"
#include "Material.h"

namespace omen {
    class Shader {
		std::string m_shaderFile;
		std::vector<std::string> includedShaders;
		std::map<std::string, GLint> uniformLocations;
		std::string readSubShader(const char* relativePath, const char*shader_file);
		std::string include_sub_shaders(const char* relativePath, const char* shader_source);
    public:
        Shader(const char*shader_file);

        void use() const;

		void listAttribs();

        bool readShaderFile(const char*shader_file);

        GLuint m_shader_program;
		GLuint m_shaderType;

        void use();

        GLint getAttribLocation(const char*attribName);

        GLint getUniformLocation(const char*uniformName);

        void setUniform1i(const char*uniform, GLint i);

        void setUniform1d(const char*uniform, double value);

        void setUniform1f(const char*uniform, float value);

        void setUniform1dv(const char*uniform, int count, double *value);

        void setUniform1fv(const char*uniform, int count, float *value);

        void setUniform1iv(const char*uniform, int count, int *value);

        void setUniform1ui(const char*uniform, unsigned int value);

        void setUniform1uiv(const char*uniform, int count, unsigned int *value);

        void setUniform2d(const char*uniform, double x, double y);

        void setUniform2dv(const char*uniform, int count, double *values);

        void setUniform2i(const char*uniform, int x, int y);

        void setUniform2iv(const char*uniform, int count, int *values);

        void setUniform2ui(const char*uniform, unsigned int x, unsigned int y);

        void setUniform2uiv(const char*uniform, int count, unsigned int *values);

        void setUniform2f(const char*uniform, float x, float y);

        void setUniform2fv(const char*uniform, int count, float *values);

        void setUniform3i(const char*uniform, int x, int y, int z);

        void setUniform3iv(const char*uniform, int count, int *values);

        void setUniform3ui(const char*uniform, unsigned int x, unsigned int y, unsigned int z);

        void setUniform3uiv(const char*uniform, int count, unsigned int *values);

        void setUniform3f(const char*uniform, float x, float y, float z);

        void setUniform3fv(const char*uniform, int count, float *values);

        void setUniform3d(const char*uniform, double x, double y, double z);

        void setUniform3dv(const char*uniform, int count, double *values);

        void setUniform4i(const char*uniform, int x, int y, int z, int w);

        void setUniform4iv(const char*uniform, int count, int *values);

        void setUniform4ui(const char*uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w);

        void setUniform4uiv(const char*uniform, int count, unsigned int *values);

        void setUniform4f(const char*uniform, float x, float y, float z, float w);

        void setUniform4fv(const char*uniform, int count, float *values);

        void setUniform4d(const char*uniform, double x, double y, double z, double w);

        void setUniform4dv(const char*uniform, int count, double *values);

        void setUniformBlockBinding(const char*uniform_block, int block_binding);

        void setUniformMatrix2fv(const char*uniform, int count, float *values, bool bTranspose);

        void setUniformMatrix2dv(const char*uniform, int count, double *values, bool bTranspose);

        void setUniformMatrix3fv(const char*uniform, int count, float *values, bool bTranspose);

        void setUniformMatrix3dv(const char*uniform, int count, double *values, bool bTranspose);

        void setUniformMatrix4fv(const char*uniform, int count, float *values, bool bTranspose);

        void setUniformMatrix4dv(const char*uniform, int count, double *values, bool bTranspose);


        void setMaterial(std::unique_ptr<Material> material);

        void setTexture(int textureIndex, const char* textureName, Texture *texture);

        std::string printShaderSource(GLuint &shader_id);

        std::string getShaderSource(GLuint &shader_id);

        bool createShader(GLenum shaderType, GLuint &shader_id, std::string shader_source);
    };
} // namespace omen

#endif //OMEN_SHADER_H
