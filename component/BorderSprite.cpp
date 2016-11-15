//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#include "BorderSprite.h"
#include "../GL_error.h"
#include "../Engine.h"

using namespace omen;
using namespace ecs;

BorderSprite::BorderSprite(const std::string& sprite, const glm::vec2& pos, float width, float height, int left, int right, int top, int bottom) 
	:
	Sprite(sprite, { pos.x,pos.y }, width, height), 
	left(left), 
	right(right), 
	top(top), 
	bottom(bottom) 
{
	setShader(new Shader("shaders/border_sprite.glsl"));
	setTexture(new Texture(sprite));
	if (m_width == -1) {
		m_width = static_cast<float>(texture()->width());
	}
	if (m_height == -1) {
		m_height = static_cast<float>(texture()->height());
	}
	m_pivot = glm::vec2(m_width / 2.0, m_height / 2.0);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_vbo_texcoord);
	glGenBuffers(1, &m_ibo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	std::vector<glm::vec3> vertices = {
			{-0.5,-0.5,0}, {0.5,-0.5,0}, {-0.5,0.5,0}, {0.5,0.5,0}
	};
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
		0/*stride*/, 0/*offset*/);


	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoord);
	std::vector<glm::vec2> texcoords = {
			{0,1}, {1,1}, {0,0}, {1,0}
	};
	glBufferData(GL_ARRAY_BUFFER, texcoords.size() * 2 * sizeof(GLfloat), texcoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
		0/*stride*/, 0/*offset*/);

	m_indices = { 0,1,2,3 };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(GLshort), m_indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}


BorderSprite::~BorderSprite() = default;

void BorderSprite::render() {
	shader()->use();
	texture()->bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glm::mat4 model(1);
	float fw = 2 * m_width / (float)Engine::instance()->window()->width();
	float fh = 2 * m_height / (float)Engine::instance()->window()->height();
	float fx = -1.0f + 2 * ((m_pos.x + m_pivot.x) / (float)Engine::instance()->window()->width());
	float fy = 1.0f - 2 * ((m_pos.y + m_pivot.y) / (float)Engine::instance()->window()->height());
	model = glm::translate(model, glm::vec3(fx, fy, 0));
	model = glm::scale(model, glm::vec3(fw, fh, 1));
	shader()->setUniformMatrix4fv("Model", 1, &model[0][0], false);
	shader()->setUniform1i("BorderLeft", left);
	shader()->setUniform1i("BorderRight", right);
	shader()->setUniform1i("BorderTop", top);
	shader()->setUniform1i("BorderBottom", bottom);
	shader()->setUniform1i("ViewWidth", m_width);
	shader()->setUniform1i("ViewHeight", m_height);
	int spriteWidth = texture()->width();
	int spriteHeight = texture()->height();
	shader()->setUniform1i("SpriteWidth", texture()->width());
	shader()->setUniform1i("SpriteHeight", texture()->height());

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/, 0/*stride*/, 0/*offset*/);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texcoord);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/, 0/*stride*/, 0/*offset*/);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void BorderSprite::initializeTexture() {
	setTexture(new Texture(sprite()));
}

void BorderSprite::initializeShader() {
	setShader(new Shader("shaders/border_sprite.glsl"));
}