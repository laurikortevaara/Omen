#include "PlainColorRenderer.h"
#include "../Entity.h"
#include "../Engine.h"
#include "../GL_error.h"
#include <glm/gtc/type_ptr.hpp>
#include "../ui/View.h"
#include "../Shader.h"

using namespace omen::ecs;

PlainColorRenderer::PlainColorRenderer(omen::Color& color) :
	Renderer(), m_color(color)
{
	setShader(std::make_unique<Shader>("shaders/plain_color.glsl"));

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_indexBuffer);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	std::vector<glm::vec3> vertices = {
		{ 0,-1,0 },{ 1,-1,0 },{ 0,0,0 },{ 1,0,0 }
	};
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,0/*stride*/, 0/*offset*/);

	std::vector<GLshort> ind = { 0,1,2,3 };
	m_indexBufferSize = 4;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferSize * sizeof(GLshort), ind.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void PlainColorRenderer::onAttach(Entity* e) 
{
	
}

void PlainColorRenderer::onDetach(Entity* e) 
{

}

void PlainColorRenderer::render(omen::Shader* shader) const
{
	m_shader->use();
	glm::mat4 model(1);
	float fw = 2.0f; 
	float fh = 2.0f; 
	float fx = -1.0f; 
	float fy = 1.0f;
	model = glm::translate(model, glm::vec3(fx, fy, 0));
	model = glm::scale(model, glm::vec3(fw, fh, 1));
	m_shader->setUniformMatrix4fv("Model", 1, &model[0][0], false);
	
	glm::vec4 c = m_color;
	m_shader->setUniform4fv("Color", 1, glm::value_ptr(c));
	m_shader->setUniform1i("Hovered", entity()->hovered());
	m_shader->setUniform1i("Pressed", entity()->pressed());


	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	drawElements(GL_TRIANGLE_STRIP, m_indexBufferSize, GL_UNSIGNED_SHORT, 0);
	m_shader->unuse();
}