//
// Created by Lauri Kortevaara on 06/02/16.
//

#include "component/TextRenderer.h"

#include "Engine.h"
#include "MathUtils.h"

#include "component/Transform.h"

#include "ui/View.h"

#include "Omen_OpenGL.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <chrono>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace omen;
using namespace ecs;

/// Holds all state information relevant to a character as loaded using FreeType
static const float FontSize = 14.0f;
static float LineHeight = 0.0f;

struct Character {
	uint32_t TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	uint32_t Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

TextRenderer::TextRenderer() :
	Renderer(),
	is_single_line(false),
	m_y_delta(0.0f),
	m_textScale(1.0f)
{
	initializeShader();
	initializeFreeType();
}

void TextRenderer::render(omen::Shader* shader) const {
	storePolygonMode();
	polygonModeFill();
	omen::ui::View* e = dynamic_cast<omen::ui::View*>(entity());

	float x = 0.0f; // e->pos2D().x;
	float y = -LineHeight; // Characters['X'].Size.y; // -e->pos2D().y;
	if (e->gravity() == omen::ui::View::VERTICAL_CENTER)
		y = -(e->pos2D().y+(e->size2D().y-Characters['X'].Size.y)*0.5f);
	renderText(m_text, x, y, m_textScale, glm::vec4(1));
	restorePolygonMode();
}

void TextRenderer::renderText(const std::wstring& text, GLfloat x, GLfloat yd, GLfloat scale, glm::vec4 color) const
{
	float y = m_y_delta + yd;
	auto start = std::chrono::high_resolution_clock::now();
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate corresponding render state	
	m_shader->use();

	omen::floatprec width = static_cast<omen::floatprec>(entity()->width());
	omen::floatprec height = static_cast<omen::floatprec>(entity()->height());

	glm::mat4 model;
	model = glm::scale(model, glm::vec3(1));
	model = glm::translate(model, glm::vec3(0, height, 0)); // height - Characters['X'].Size.y, 0.0f));
	m_shader->setUniformMatrix4fv("Model", 1, glm::value_ptr(model), false);
	m_shader->setUniform4fv("TextColor", 1, &color[0]);

	
	glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height);
	float w = static_cast<float>(Engine::instance()->window()->width());
	float h = static_cast<float>(Engine::instance()->window()->height());
	//glm::mat4 projection = glm::ortho(0.0f, w, 0.0f, h);
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
		Character ch = Characters[static_cast<const char>(*c)];
		//std::cout << "character: " << (int)*c << " is " << (char)*c << std::endl;
		if (*c == '\n' || *c == 9) {
			y -= (Characters['X'].Size.y + row_spacing)*scale;
			x = origin_x;
			continue;
		}

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		if (is_single_line)
		{
			if (xpos + Characters['.'].Size.x * 3.0f > m_entity->size().x) {
				ch = Characters['.'];
				xpos = x + ch.Bearing.x * scale;
				ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

				w = ch.Size.x * scale;
				h = ch.Size.y * scale;
			}
			if (xpos + w > (m_entity->pos().x + m_entity->size().x))
				continue;
		}
		else
		{
			if (xpos + Characters['.'].Size.x * 3.0f > m_entity->size().x) {
				x = 0;
				y -= LineHeight;
				xpos = x + ch.Bearing.x * scale;
				ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
				
			}
		}
		
		

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
		drawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	double ms = diff.count() * 1000.0f;
	//std::cout << "Text render: " << ms << "ms.\n";
}

glm::vec2 TextRenderer::getTextBounds()
{
	float y = 0.0f;
	float x = 0.0f;
	float maxx = 0.0f;
	float maxy = 0.0f;
	omen::floatprec width = static_cast<omen::floatprec>(entity()->width());
	omen::floatprec height = static_cast<omen::floatprec>(entity()->height());

	float w = static_cast<float>(Engine::instance()->window()->width());
	float h = static_cast<float>(Engine::instance()->window()->height());

	// Iterate through all characters
	std::wstring::const_iterator c;
	omen::floatprec origin_x = 0;
	omen::floatprec row_spacing = Characters['X'].Size.y * 0.5f;

	for (c = m_text.begin(); c != m_text.end(); c++)
	{
		Character ch = Characters[static_cast<const char>(*c)];
		if (*c == '\n' || *c == 9) {
			y -= (Characters['X'].Size.y + row_spacing)*m_textScale;
			x = origin_x;
			continue;
		}

		GLfloat xpos = x + ch.Bearing.x * m_textScale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * m_textScale;

		GLfloat w = ch.Size.x * m_textScale;
		GLfloat h = ch.Size.y * m_textScale;

		if (is_single_line)
		{
			if (xpos + Characters['.'].Size.x * 3.0f > m_entity->size().x) {
				ch = Characters['.'];
				xpos = x + ch.Bearing.x * m_textScale;
				ypos = y - (ch.Size.y - ch.Bearing.y) * m_textScale;

				w = ch.Size.x * m_textScale;
				h = ch.Size.y * m_textScale;
			}
			if (xpos + w > (m_entity->pos().x + m_entity->size().x))
				continue;
		}
		else
		{
			if (xpos + Characters['.'].Size.x * 3.0f > m_entity->size().x) {
				x = 0;
				y -= LineHeight;
				xpos = x + ch.Bearing.x * m_textScale;
				ypos = y - (ch.Size.y - ch.Bearing.y) * m_textScale;

			}
		}

		maxx = glm::max<float>(xpos, maxx);
		maxy = glm::max<float>(-ypos, maxy);

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * m_textScale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}

	return { maxx,maxy+LineHeight };
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

    FT_Set_Pixel_Sizes(m_fontFace, 0, static_cast<FT_UInt>(FontSize));

    if (FT_Load_Char(m_fontFace, 'X', FT_LOAD_RENDER)) {
        std::cerr << "Could not load character 'X'" << std::endl;
        return false;
    }

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 255; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(m_fontFace, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		uint32_t texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		LineHeight = FontSize*(m_fontFace->height / (float)m_fontFace->units_per_EM);
		//LineHeight = m_fontFace->size->metrics.height;
		
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
			static_cast<uint32_t>(m_fontFace->glyph->advance.x)
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

void TextRenderer::setText(const std::wstring& text)
{
	m_text = text;
	invalidate();
}

void TextRenderer::invalidate()
{
	glm::vec2 bounds = getTextBounds();
	if (bounds.y>entity()->height())
		Engine::instance()->signal_engine_update.connect(this, [&](omen::floatprec time, omen::floatprec dt) {
		m_y_delta = bounds.y*sin(time);
	});
	else
	{
		Engine::instance()->signal_engine_update.removeObserver(this);
		m_y_delta = 0.0f;
	}
}