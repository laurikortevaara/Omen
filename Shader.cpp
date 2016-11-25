//
// Created by Lauri Kortevaara on 21/12/15.
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

#include <boxer/boxer.h>

#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <memory>

#include "GL_error.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "utils.h"
#include "Engine.h"
#include "component/MouseInput.h"

using namespace omen;

Shader::Shader(const std::string &shader_file) : m_shaderFile(shader_file) {
	readShaderFile(shader_file);

}

bool Shader::createShader(GLenum shaderType, GLuint &shader_id, std::string shader_source) {
	std::string full_source;
	GLuint shader = 0;

	switch (shaderType) {
	case GL_VERTEX_SHADER:
		full_source = "#version 410\n#define VERTEX_SHADER\n";
		break;
	case GL_GEOMETRY_SHADER:
		full_source = "#version 410\n#define GEOMETRY_SHADER\n";
		break;
	case GL_TESS_CONTROL_SHADER:
		full_source = "#version 410\n#define TESS_CONTROL_SHADER\n";
		break;
	case GL_TESS_EVALUATION_SHADER:
		full_source = "#version 410\n#define TESS_EVALUATION_SHADER\n";
		break;
	case GL_FRAGMENT_SHADER:
		full_source = "#version 410\n#define FRAGMENT_SHADER\n";
		break;
	default:
		break;
	}

	full_source += "uniform float iGlobalTime;"; // Floating point global time in seconds
	full_source += "uniform vec4 iMouse;"; // MouseCoord(x,y), MouseButtons(Left,Right);
	full_source += "uniform vec2 iResolution;"; // WindowSize(width,height);
	full_source += shader_source;
	full_source += "\0";

	shader = glCreateShader(shaderType);
	check_gl_error();


	unsigned long len = full_source.length()+1;
	char *shader_source_buffer = new char[len];
	strcpy_s(shader_source_buffer, len, full_source.c_str());
	glShaderSource(shader, 1, (const char *const *)&shader_source_buffer, nullptr);
	check_gl_error();

	GLchar info_log[1024];
	GLchar shader_source_raw[100 * 1024];
	GLsizei shader_len = 0;

	glCompileShader(shader);
	check_gl_error();

	glGetShaderSource(shader, sizeof(shader_source_raw), &shader_len, &shader_source_raw[0]);

	int info_len = 0;
	glGetShaderInfoLog(shader, sizeof(info_log), &info_len, info_log);
	if (strlen(info_log) != 0) {
		boxer::show(info_log, "Vertex Shader ERROR");
		std::string str_info_log = info_log;
		int i1 = str_info_log.find_first_of("(");
		int i2 = str_info_log.find_first_of("(", i1 + 1);
		std::string line_no = str_info_log.substr(i1 + 1, i2-1);
		int line_number = std::stoi(line_no);
		std::vector<std::string> lines = omen::split_string(shader_source_raw, "\n");
		std::string source;
		int line = 1;
		for (auto l : lines) {
			if(line> (line_number - 10) && line <= (line_number + 10))
				source += std::to_string(line) + ": " + l;
			line++;
		}
		boxer::show(source.c_str(), m_shaderFile.c_str());
			
		return false;
	}
	shader_id = shader;
	return true;
}

std::string Shader::getShaderSource(GLuint &shader_id) {
	GLchar vertex_shader_source[1024];
	memset(vertex_shader_source, 0, 1024);
	GLsizei vertex_shader_source_len = 0;
	glGetShaderSource(shader_id, sizeof(vertex_shader_source), &vertex_shader_source_len, vertex_shader_source);
	return vertex_shader_source;
}

std::string Shader::readSubShader(const std::string& relativePath, const std::string &shader_file) {
	std::string fullPath = relativePath + shader_file;
	std::ifstream is(fullPath, std::ifstream::binary);
	if (is && std::find(includedShaders.begin(), includedShaders.end(), fullPath ) == includedShaders.end()) {
		std::ostringstream sout;
		std::copy(std::istreambuf_iterator<char>(is),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(sout));
		is.close();

		includedShaders.push_back(fullPath);
		return sout.str();
	}
	return "";
}

std::string Shader::include_sub_shaders(const std::string& relativePath, const std::string& shader_source) 
{
	std::vector<std::string> lines = omen::split_string(shader_source, "\n");
	std::string full_shader;
	for (auto& line : lines)
	{
		if (line.find("#include") == std::string::npos) {
			full_shader += line +"\n";
			continue;
		}
		int i0 = line.find_first_of("\"")+1;
		//for (int i = 0; i < 100; ++i) {
		//	std::string s = line.substr(i0, i);
		//	std::cout << s << std::endl;
		//}
		int i1 = line.find_first_of("\"", i0);
		std::string filename = line.substr(i0, i1-i0);
		
		full_shader += readSubShader(relativePath, filename) + "\n";
	}
	return full_shader;
}

bool Shader::readShaderFile(const std::string &shader_file) {
	std::ifstream is(shader_file, std::ifstream::binary);
	if (is) {
		std::ostringstream sout;
		std::copy(std::istreambuf_iterator<char>(is),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(sout));
		is.close();

		m_shader_program = glCreateProgram();
		check_gl_error();

		std::string shader_source = sout.str();
		std::string relativePath = shader_file.find_first_of("/") == std::string::npos ? "./" :
			shader_file.substr(0, shader_file.find_last_of("/")+1);

		shader_source = include_sub_shaders(relativePath,shader_source);

		GLuint vShader, gShader, tcShader, teShader, fShader;
		if (shader_source.find("VERTEX_SHADER") != std::string::npos) {
			if (createShader(GL_VERTEX_SHADER, vShader, shader_source))
				glAttachShader(m_shader_program, vShader);
		}
		if (shader_source.find("GEOMETRY_SHADER") != std::string::npos) {
			if (createShader(GL_GEOMETRY_SHADER, gShader, shader_source))
				glAttachShader(m_shader_program, gShader);
		}
		if (shader_source.find("TESS_CONTROL_SHADER") != std::string::npos) {
			if (createShader(GL_TESS_CONTROL_SHADER, tcShader, shader_source))
				glAttachShader(m_shader_program, tcShader);
		}
		if (shader_source.find("TESS_EVALUATION_SHADER") != std::string::npos) {
			if (createShader(GL_TESS_EVALUATION_SHADER, teShader, shader_source))
				glAttachShader(m_shader_program, teShader);
		}
		if (shader_source.find("FRAGMENT_SHADER") != std::string::npos) {
			if (createShader(GL_FRAGMENT_SHADER, fShader, shader_source))
				glAttachShader(m_shader_program, fShader);
		}

		glLinkProgram(m_shader_program);

		GLchar info_log[1024];
		int info_len = 0;
		glGetProgramInfoLog(m_shader_program, sizeof(info_log), &info_len, info_log);
		if (strlen(info_log) != 0) {
			boxer::show(info_log, "Error");
		}
	}


	return true;
}

void Shader::use() {
	check_gl_error();
	glUseProgram(m_shader_program);
	setUniform1f("iGlobalTime", Engine::instance()->time());
	glm::vec2 mousePos = Engine::instance()->findComponent<MouseInput>()->cursorPos();
	glm::vec2 mouseButtons = Engine::instance()->findComponent<MouseInput>()->mouseButtonStatesLR();
	glm::vec4 iMouse(mousePos, mouseButtons);
	setUniform4fv("iMouse", 1, glm::value_ptr(mousePos));

	glm::vec2 windowSize(Engine::instance()->window()->width(), Engine::instance()->window()->height());
	setUniform2fv("iResolution", 1, glm::value_ptr(windowSize));

	check_gl_error();
}


/**
 * 1D
 */
void Shader::setUniform1i(const std::string &uniform, GLint value) {
	if (getUniformLocation(uniform) >= 0) {
		GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
		glUniform1i(uniform_loc, value);
	}
}

void Shader::setUniform1iv(const std::string &uniform, int count, int *value) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform1iv(uniform_loc, count, value);
}

void Shader::setUniform1ui(const std::string &uniform, unsigned int value) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform1ui(uniform_loc, value);
}

void Shader::setUniform1uiv(const std::string &uniform, int count, unsigned int *value) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform1uiv(uniform_loc, count, value);
}

void Shader::setUniform1f(const std::string &uniform, float value) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform1f(uniform_loc, value);
}

void Shader::setUniform1fv(const std::string &uniform, int count, float *value) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform1fv(uniform_loc, count, value);
}

void Shader::setUniform1d(const std::string &uniform, double value) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform1d(uniform_loc, value);
}

void Shader::setUniform1dv(const std::string &uniform, int count, double *value) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform1dv(uniform_loc, count, value);
}

/**
 * 2D
 */
void Shader::setUniform2i(const std::string &uniform, int x, int y) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform2i(uniform_loc, x, y);
}

void Shader::setUniform2iv(const std::string &uniform, int count, int *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform2iv(uniform_loc, count, values);
}

void Shader::setUniform2ui(const std::string &uniform, unsigned int x, unsigned int y) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform2ui(uniform_loc, x, y);
}

void Shader::setUniform2uiv(const std::string &uniform, int count, unsigned int *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform2uiv(uniform_loc, count, values);
}

void Shader::setUniform2f(const std::string &uniform, float x, float y) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform2f(uniform_loc, x, y);
}

void Shader::setUniform2fv(const std::string &uniform, int count, float *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform2fv(uniform_loc, count, values);
}

void Shader::setUniform2d(const std::string &uniform, double x, double y) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform2d(uniform_loc, x, y);
}

void Shader::setUniform2dv(const std::string &uniform, int count, double *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform2dv(uniform_loc, count, values);
}

/**
 * 3D
 */
void Shader::setUniform3i(const std::string &uniform, int x, int y, int z) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform3i(uniform_loc, x, y, z);
}

void Shader::setUniform3iv(const std::string &uniform, int count, int *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform3iv(uniform_loc, count, values);
}

void Shader::setUniform3ui(const std::string &uniform, unsigned int x, unsigned int y, unsigned int z) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform3ui(uniform_loc, x, y, z);
}

void Shader::setUniform3uiv(const std::string &uniform, int count, unsigned int *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform3uiv(uniform_loc, count, values);
}

void Shader::setUniform3f(const std::string &uniform, float x, float y, float z) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform3f(uniform_loc, x, y, z);
}

void Shader::setUniform3fv(const std::string &uniform, int count, float *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform3fv(uniform_loc, count, values);
}

void Shader::setUniform3d(const std::string &uniform, double x, double y, double z) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform3d(uniform_loc, x, y, z);
}

void Shader::setUniform3dv(const std::string &uniform, int count, double *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform3dv(uniform_loc, count, values);
}


/**
 * 4D
 */
void Shader::setUniform4i(const std::string &uniform, int x, int y, int z, int w) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform4i(uniform_loc, x, y, z, w);
}

void Shader::setUniform4iv(const std::string &uniform, int count, int *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform4iv(uniform_loc, count, values);
}

void Shader::setUniform4ui(const std::string &uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform4ui(uniform_loc, x, y, z, w);
}

void Shader::setUniform4uiv(const std::string &uniform, int count, unsigned int *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform4uiv(uniform_loc, count, values);
}

void Shader::setUniform4f(const std::string &uniform, float x, float y, float z, float w) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform4f(uniform_loc, x, y, z, w);
}

void Shader::setUniform4fv(const std::string &uniform, int count, float *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform4fv(uniform_loc, count, values);
}

void Shader::setUniform4d(const std::string &uniform, double x, double y, double z, double w) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform4d(uniform_loc, x, y, z, w);
}

void Shader::setUniform4dv(const std::string &uniform, int count, double *values) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniform4dv(uniform_loc, count, values);
}


/**
 * Uniform block binding
 */
void Shader::setUniformBlockBinding(const std::string &uniform_block, int block_binding) {
	GLint block_index = glGetUniformBlockIndex(m_shader_program, uniform_block.c_str());
	glUniformBlockBinding(m_shader_program, block_index, block_binding);
}


/**
 * Matrix uniforms
 */
void Shader::setUniformMatrix2fv(const std::string &uniform, int count, float *values, bool bTranspose) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniformMatrix2fv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix2dv(const std::string &uniform, int count, double *values, bool bTranspose) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniformMatrix2dv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix3fv(const std::string &uniform, int count, float *values, bool bTranspose) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniformMatrix3fv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix3dv(const std::string &uniform, int count, double *values, bool bTranspose) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniformMatrix3dv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix4fv(const std::string &uniform, int count, float *values, bool bTranspose) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniformMatrix4fv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix4dv(const std::string &uniform, int count, double *values, bool bTranspose) {
	GLint uniform_loc = glGetUniformLocation(m_shader_program, uniform.c_str());
	glUniformMatrix4dv(uniform_loc, count, bTranspose, values);
}


GLint Shader::getAttribLocation(const std::string &attribName) {
	return glGetAttribLocation(m_shader_program, attribName.c_str());
}

GLint Shader::getUniformLocation(const std::string &uniformName) {
	int loc = glGetUniformLocation(m_shader_program, uniformName.c_str());
	return loc;
}


void Shader::setTexture(int textureIndex, const std::string &textureName, Texture *texture) {
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	int loc = glGetUniformLocation(m_shader_program, "Texture");
	glUniform1i(loc, GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->id());
}

void Shader::setMaterial(std::unique_ptr<Material> material) {
	/*if(material->texture()!= nullptr)
		setTexture(0, "Texture", material->texture());*/
	use();
}
