//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#include "Sprite.h"
#include "../GL_error.h"
#include "../Engine.h"

using namespace omen;
using namespace ecs;

Sprite::Sprite(const std::string& sprite, const glm::vec2& pos, float width, float height) :
	Renderable({ pos.x,pos.y,0 }, width, height, 0), m_sprite(sprite) {
	setShader(new Shader("shaders/sprite.glsl"));
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


Sprite::~Sprite() = default;

void Sprite::render() {
	shader()->use();
	texture()->bind();

	glDisable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
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
	glEnable(GL_DEPTH_TEST);
}

/*void Sprite::onAttach(ecs::Entity *e) {
	m_entity = e;
	Transform* tr = new Transform;
	tr->pos() = glm::vec3(m_pos, 0);
	omen::floatprec bminx = -(m_width / 2.0f) + m_pivot.x;
	omen::floatprec bmaxx = (m_width / 2.0f) + m_pivot.x;
	omen::floatprec bminy = -(m_height / 2.0f) + m_pivot.y;
	omen::floatprec bmaxy = (m_height / 2.0f) + m_pivot.y;
	tr->setBounds(glm::vec3(bminx, bminy, 0), glm::vec3(bmaxx, bmaxy, 0));
	m_entity->addComponent(tr);
}

void Sprite::onDetach(ecs::Entity *e) {
	m_entity = nullptr;
}
*/
void Sprite::initializeTexture() {
	setTexture(new Texture(m_sprite));
}

void Sprite::initializeShader() {
	setShader(new Shader("shaders/sprite.glsl"));
}
