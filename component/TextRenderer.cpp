//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Omen_OpenGL.h"
#include "TextRenderer.h"
#include "../component/Transform.h"
#include "../GL_error.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace omen;
using namespace ecs;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

TextRenderer::TextRenderer() :
	Renderer()
{
	initializeShader();
	initializeFreeType();
}

void TextRenderer::render(Shader* shader) {
	renderText(m_text, 10, -200, 1.0, glm::vec4(1));
}

void TextRenderer::renderText(const std::wstring& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate corresponding render state	
	m_shader->use();

	float width = Engine::instance()->window()->width();
	float height = Engine::instance()->window()->height();

	glm::mat4 model;
	model = glm::scale(model, glm::vec3(1));
	model = glm::translate(model, glm::vec3(0, height-Characters['X'].Size.y, 0.0f));
	m_shader->setUniformMatrix4fv("Model", 1, glm::value_ptr(model), false);
	m_shader->setUniform4fv("TextColor", 1, &color[0]);

	glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height);
	m_shader->setUniformMatrix4fv("Projection", 1, glm::value_ptr(projection), false);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_vao);

	// Iterate through all characters
	std::wstring::const_iterator c;
	omen::floatprec origin_x = x;
	omen::floatprec row_spacing = Characters['X'].Size.y * 0.5f;

	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		if (*c == '\n') {
			y -= (Characters['X'].Size.y + row_spacing)*scale;
			x = origin_x;
			continue;
		}
		
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool TextRenderer::initializeFreeType() {
    if (FT_Init_FreeType(&m_freetype)) {
        std::cerr << "Could not init freetype library" << std::endl;
        return false;
    }

    if (FT_New_Face(m_freetype, "fonts/Roboto-Light.ttf", 0, &m_fontFace)) {
        std::cerr << "Could not open font" << std::endl;
        return false;
    }
	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    FT_Set_Pixel_Sizes(m_fontFace, 0, 24);

    if (FT_Load_Char(m_fontFace, 'X', FT_LOAD_RENDER)) {
        std::cerr << "Could not load character 'X'" << std::endl;
        return false;
    }

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(m_fontFace, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			m_fontFace->glyph->bitmap.width,
			m_fontFace->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			m_fontFace->glyph->bitmap.buffer
			);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(m_fontFace->glyph->bitmap.width, m_fontFace->glyph->bitmap.rows),
			glm::ivec2(m_fontFace->glyph->bitmap_left, m_fontFace->glyph->bitmap_top),
			m_fontFace->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(m_fontFace);
	FT_Done_FreeType(m_freetype);
	
	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    return true;

}

void TextRenderer::initializeShader() {
    setShader(std::make_unique<Shader>("shaders/text_shader.glsl"));
}

void TextRenderer::initializeTexture() {
    setTexture(nullptr);
}
