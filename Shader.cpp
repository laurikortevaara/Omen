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
#include "StringTools.h"

using namespace omen;

Shader::Shader(const char*shader_file) : m_shaderFile(shader_file) {
	readShaderFile(shader_file);

}

bool Shader::createShader(GLenum shaderType, GLuint &shader_id, std::string shader_source) {
	m_shaderType = shaderType;

	std::string full_source;
	GLuint shader = 0;

	switch (shaderType) {
	case GL_VERTEX_SHADER:
		full_source = "#version 430\n#define VERTEX_SHADER\n";
		break;
	case GL_GEOMETRY_SHADER:
		full_source = "#version 430\n#define GEOMETRY_SHADER\n";
		break;
	case GL_TESS_CONTROL_SHADER:
		full_source = "#version 430\n#define TESS_CONTROL_SHADER\n";
		break;
	case GL_TESS_EVALUATION_SHADER:
		full_source = "#version 430\n#define TESS_EVALUATION_SHADER\n";
		break;
	case GL_FRAGMENT_SHADER:
		full_source = "#version 430\n#define FRAGMENT_SHADER\n";
		break;
	case GL_COMPUTE_SHADER:
		full_source = "#version 430\n#define COMPUTE_SHADER\n";
		break;
	default:
		break;
	}

	if (shaderType != GL_COMPUTE_SHADER)
	{
		full_source += "uniform float iGlobalTime;"; // Floating point global time in seconds
		full_source += "uniform vec4 iMouse;"; // MouseCoord(x,y), MouseButtons(Left,Right);
		full_source += "uniform vec2 iResolution;"; // WindowSize(width,height);
		full_source += "uniform vec3 iMousePickRay;"; // WindowSize(width,height);
		full_source += "uniform vec3 iCameraPosition;"; // WindowSize(width,height);
		full_source += "uniform float MATH_PI;"; // WindowSize(width,height);
		full_source += "uniform vec4 iViewPort;"; // Viewport [x,y,z,w];
	}
	if (shaderType == GL_TESS_CONTROL_SHADER)
	{
		//if (shader_source.find("InnerTessellationLevel1") == std::string::npos)
		full_source += "uniform float InnerTessellationLevel1;";
		//if (shader_source.find("InnerTessellationLevel2") == std::string::npos)
		full_source += "uniform float InnerTessellationLevel2;";
		//if (shader_source.find("OuterTessellationLevel1") == std::string::npos)
		full_source += "uniform float OuterTessellationLevel1;";
		//if (shader_source.find("OuterTessellationLevel2") == std::string::npos)
		full_source += "uniform float OuterTessellationLevel2;";
		//if (shader_source.find("OuterTessellationLevel3") == std::string::npos)
		full_source += "uniform float OuterTessellationLevel3;";
		//if (shader_source.find("OuterTessellationLevel4") == std::string::npos)
		full_source += "uniform float OuterTessellationLevel4;";
	}
	full_source += shader_source;
	full_source += "\0";

	shader = glCreateShader(shaderType);
	check_gl_error();


	unsigned long len = full_source.length()+1;
	char *shader_source_buffer = new char[len];
	strcpy_s(shader_source_buffer, len, full_source.c_str());

	//std::cout << "shader source: " << shader_source_buffer << "\n--------------------------------------------------------------------\n";

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

std::string Shader::readSubShader(const char* relativePath, const char*shader_file) {
	std::string fullPath = relativePath + std::string(shader_file);
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

std::string Shader::include_sub_shaders(const char* relativePath, const char* shader_source) 
{
	std::vector<std::string> lines = omen::split_string(shader_source, "\n");
	std::string full_shader;
	bool comment_section = false;
	for (auto& line : lines)
	{
		while (line.find('\r') != std::string::npos)
			line = line.erase(line.find('\r'));
		line = StripComments(line);
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
		
		full_shader += readSubShader(relativePath, filename.c_str()) + "\n";
	}
	return full_shader;
}

bool Shader::readShaderFile(const char*shader_file) {
	std::cout << "Loading shader from file: " << shader_file << std::endl;
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
		std::string str_shader_file = shader_file;
		std::string relativePath = str_shader_file.find_first_of("/") == std::string::npos ? "./" :
			str_shader_file.substr(0, str_shader_file.find_last_of("/")+1);

		shader_source = include_sub_shaders(relativePath.c_str(),shader_source.c_str());

		GLuint vShader, gShader, tcShader, teShader, fShader, cShader;
		if (shader_source.find("VERTEX_SHADER") != std::string::npos) {
			if (createShader(GL_VERTEX_SHADER, vShader, shader_source))
				glAttachShader(m_shader_program, vShader);
		}
		if (shader_source.find("GEOMETRY_SHADER") != std::string::npos) {
			if (createShader(GL_GEOMETRY_SHADER, gShader, shader_source))
				glAttachShader(m_shader_program, gShader);
		}
		if (shader_source.find("TESS_CONTROL_SHADER") != std::string::npos) {
			if (createShader(GL_TESS_CONTROL_SHADER, tcShader, shader_source)) {
				glAttachShader(m_shader_program, tcShader);
				setUniform1f("InnerTessellationLevel1", 1.0f);
				setUniform1f("InnerTessellationLevel2", 1.0f);
				setUniform1f("OuterTessellationLevel1", 1.0f);
				setUniform1f("OuterTessellationLevel2", 1.0f);
				setUniform1f("OuterTessellationLevel3", 1.0f);
				setUniform1f("OuterTessellationLevel4", 1.0f);
			}
		}
		if (shader_source.find("TESS_EVALUATION_SHADER") != std::string::npos) {
			if (createShader(GL_TESS_EVALUATION_SHADER, teShader, shader_source))
				glAttachShader(m_shader_program, teShader);
		}
		if (shader_source.find("FRAGMENT_SHADER") != std::string::npos) {
			if (createShader(GL_FRAGMENT_SHADER, fShader, shader_source))
				glAttachShader(m_shader_program, fShader);
		}
		if (shader_source.find("COMPUTE_SHADER") != std::string::npos) {
			if (createShader(GL_COMPUTE_SHADER, cShader, shader_source)) {
				glAttachShader(m_shader_program, cShader);
			}
		}

		glLinkProgram(m_shader_program);

		GLchar info_log[1024];
		int info_len = 0;
		glGetProgramInfoLog(m_shader_program, sizeof(info_log), &info_len, info_log);
		if (strlen(info_log) != 0) {
			boxer::show(info_log, "Error");
		}

		if (shader_source.find("COMPUTE_SHADER") != std::string::npos) {
			glUniform1i(glGetUniformLocation(m_shader_program, "img_output"), 0);
		}
	}
	check_gl_error();
	listAttribs();
	std::cout << "--- Done Loading shader from file: " << shader_file << "---" << std::endl;
	return true;
}

void Shader::use() {
	check_gl_error();
	if (!glIsProgram(m_shader_program))
		abort();
	glUseProgram(m_shader_program);
	//if (m_shaderType == GL_COMPUTE_SHADER)
//		return;
	setUniform1f("iGlobalTime", Engine::instance()->time());
	glm::vec2 mousePos = Engine::instance()->findComponent<MouseInput>()->cursorPos();
	glm::vec2 mouseButtons = Engine::instance()->findComponent<MouseInput>()->mouseButtonStatesLR();
	glm::vec4 iMouse(mousePos, mouseButtons);
	setUniform4fv("iMouse", 1, glm::value_ptr(mousePos));

	glm::vec2 windowSize(Engine::instance()->window()->width(), Engine::instance()->window()->height());
	setUniform2fv("iResolution", 1, glm::value_ptr(windowSize));

	GLint vport[4];
	glGetIntegerv(GL_VIEWPORT, vport);
	setUniform4iv("iViewPort", 1, vport);

	setUniform3fv("iMousePickRay", 1, glm::value_ptr(Engine::MousePickRay));

	glm::vec3 cameraPosition = Engine::instance()->camera()->position();
	cameraPosition.x *= -1.0f;
	setUniform3fv("iCameraPosition", 1, glm::value_ptr(cameraPosition));

	setUniform1f("MATH_PI", 3.141592653589793238462643383279502884f);

	check_gl_error();
}


/**
 * 1D
 */
void Shader::setUniform1i(const char*uniform, GLint value) {
	if (getUniformLocation(uniform) >= 0) {
		GLint uniform_loc = getUniformLocation(uniform);
		if (uniform_loc >= 0)
			glUniform1i(uniform_loc, value);
	}
}

void Shader::setUniform1iv(const char*uniform, int count, int *value) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform1iv(uniform_loc, count, value);
}

void Shader::setUniform1ui(const char*uniform, unsigned int value) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform1ui(uniform_loc, value);
}

void Shader::setUniform1uiv(const char*uniform, int count, unsigned int *value) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform1uiv(uniform_loc, count, value);
}

void Shader::setUniform1f(const char*uniform, float value) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform1f(uniform_loc, value);
}

void Shader::setUniform1fv(const char*uniform, int count, float *value) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform1fv(uniform_loc, count, value);
}

void Shader::setUniform1d(const char*uniform, double value) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform1d(uniform_loc, value);
}

void Shader::setUniform1dv(const char*uniform, int count, double *value) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform1dv(uniform_loc, count, value);
}

/**
 * 2D
 */
void Shader::setUniform2i(const char*uniform, int x, int y) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform2i(uniform_loc, x, y);
}

void Shader::setUniform2iv(const char*uniform, int count, int *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform2iv(uniform_loc, count, values);
}

void Shader::setUniform2ui(const char*uniform, unsigned int x, unsigned int y) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform2ui(uniform_loc, x, y);
}

void Shader::setUniform2uiv(const char*uniform, int count, unsigned int *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform2uiv(uniform_loc, count, values);
}

void Shader::setUniform2f(const char*uniform, float x, float y) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform2f(uniform_loc, x, y);
}

void Shader::setUniform2fv(const char*uniform, int count, float *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform2fv(uniform_loc, count, values);
}

void Shader::setUniform2d(const char*uniform, double x, double y) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform2d(uniform_loc, x, y);
}

void Shader::setUniform2dv(const char*uniform, int count, double *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform2dv(uniform_loc, count, values);
}

/**
 * 3D
 */
void Shader::setUniform3i(const char*uniform, int x, int y, int z) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform3i(uniform_loc, x, y, z);
}

void Shader::setUniform3iv(const char*uniform, int count, int *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform3iv(uniform_loc, count, values);
}

void Shader::setUniform3ui(const char*uniform, unsigned int x, unsigned int y, unsigned int z) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform3ui(uniform_loc, x, y, z);
}

void Shader::setUniform3uiv(const char*uniform, int count, unsigned int *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform3uiv(uniform_loc, count, values);
}

void Shader::setUniform3f(const char*uniform, float x, float y, float z) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform3f(uniform_loc, x, y, z);
}

void Shader::setUniform3fv(const char*uniform, int count, float *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform3fv(uniform_loc, count, values);
}

void Shader::setUniform3d(const char*uniform, double x, double y, double z) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform3d(uniform_loc, x, y, z);
}

void Shader::setUniform3dv(const char*uniform, int count, double *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform3dv(uniform_loc, count, values);
}


/**
 * 4D
 */
void Shader::setUniform4i(const char*uniform, int x, int y, int z, int w) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform4i(uniform_loc, x, y, z, w);
}

void Shader::setUniform4iv(const char*uniform, int count, int *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform4iv(uniform_loc, count, values);
}

void Shader::setUniform4ui(const char*uniform, unsigned int x, unsigned int y, unsigned int z, unsigned int w) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform4ui(uniform_loc, x, y, z, w);
}

void Shader::setUniform4uiv(const char*uniform, int count, unsigned int *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform4uiv(uniform_loc, count, values);
}

void Shader::setUniform4f(const char*uniform, float x, float y, float z, float w) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform4f(uniform_loc, x, y, z, w);
}

void Shader::setUniform4fv(const char*uniform, int count, float *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform4fv(uniform_loc, count, values);
}

void Shader::setUniform4d(const char*uniform, double x, double y, double z, double w) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform4d(uniform_loc, x, y, z, w);
}

void Shader::setUniform4dv(const char*uniform, int count, double *values) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniform4dv(uniform_loc, count, values);
}


/**
 * Uniform block binding
 */
void Shader::setUniformBlockBinding(const char*uniform_block, int block_binding) {
	GLint block_index = glGetUniformBlockIndex(m_shader_program, uniform_block);
	if(block_index >= 0 )
		glUniformBlockBinding(m_shader_program, block_index, block_binding);
}


/**
 * Matrix uniforms
 */
void Shader::setUniformMatrix2fv(const char*uniform, int count, float *values, bool bTranspose) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniformMatrix2fv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix2dv(const char*uniform, int count, double *values, bool bTranspose) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniformMatrix2dv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix3fv(const char*uniform, int count, float *values, bool bTranspose) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniformMatrix3fv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix3dv(const char*uniform, int count, double *values, bool bTranspose) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniformMatrix3dv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix4fv(const char*uniform, int count, float *values, bool bTranspose) {
	GLint uniform_loc = getUniformLocation(uniform);
	if (uniform_loc >= 0)
		glUniformMatrix4fv(uniform_loc, count, bTranspose, values);
}

void Shader::setUniformMatrix4dv(const char*uniform, int count, double *values, bool bTranspose) {
	GLint uniform_loc = getUniformLocation(uniform);
	if(uniform_loc >= 0)
		glUniformMatrix4dv(uniform_loc, count, bTranspose, values);
}


GLint Shader::getAttribLocation(const char*attribName) {
	return glGetAttribLocation(m_shader_program, attribName);
}

void Shader::listAttribs()
{
	GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 16; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length

	// Attributes
	glGetProgramiv(m_shader_program, GL_ACTIVE_ATTRIBUTES, &count);
	printf("Active Attributes: %d\n", count);

	for (i = 0; i < count; i++)
	{
		glGetActiveAttrib(m_shader_program, (GLuint)i, bufSize, &length, &size, &type, name);

		printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
	}

	// Uniforms
	glGetProgramiv(m_shader_program, GL_ACTIVE_UNIFORMS, &count);
	printf("Active Uniforms: %d\n", count);

	for (i = 0; i < count; i++)
	{
		glGetActiveUniform(m_shader_program, (GLuint)i, bufSize, &length, &size, &type, name);

		std::string strType = "";
		switch (type)
		{
		case GL_SAMPLER_2D:
			strType = "GL_SAMPLER_2D";
			break;
		default:
			strType = std::to_string(type);
		}

		printf("Uniform #%d Type: %s Name: %s\n", i, strType.c_str(), name);
	}
}
GLint Shader::getUniformLocation(const char*uniformName) {
	//std::map<std::string, GLint>::iterator iter = uniformLocations.find(uniformName);
	GLint loc = glGetUniformLocation(m_shader_program, uniformName);
	/*if (iter == uniformLocations.end()) {
		loc = glGetUniformLocation(m_shader_program, uniformName.c_str());
		uniformLocations[uniformName] = loc;
	}
	else
		loc = uniformLocations[uniformName];*/

	return loc;
}


void Shader::setTexture(int textureIndex, const char*textureName, Texture *texture) {
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	int loc = glGetUniformLocation(m_shader_program, "Texture");
	if (loc >= 0)
		glUniform1i(loc, GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->id());
}

void Shader::setMaterial(std::unique_ptr<Material> material) {
	/*if(material->texture()!= nullptr)
		setTexture(0, "Texture", material->texture());*/
	use();
}
