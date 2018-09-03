//
// Created by Lauri Kortevaara on 11/01/16.
//

#include "component/Sprite.h"

#include "Engine.h"
#include "component/Renderer.h"
#include "Omen_OpenGL.h"

using namespace omen;
using namespace ecs;

Sprite::Sprite(const std::string& sprite, const glm::vec2& pos, const glm::vec2& size) :
	Renderable({ pos.x,pos.y }, size.x, size.y, 0), m_sprite(sprite) {
	setShader(new Shader("shaders/sprite.glsl"));
	setTexture(new Texture(sprite));
	if (width() == -1) {
		setWidth(static_cast<float>(texture()->width()));
	}
	if (height() == -1) {
		setHeight(static_cast<float>(texture()->height()));
	}
	setPivot(glm::vec2(width() / 2.0, height() / 2.0));
	setPivot(glm::vec2(0,0));

	uint32_t vao, vbo, vbo_texcoord, ibo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbo_texcoord);
	glGenBuffers(1, &ibo);

	setVao(vao);
	setVbo(vbo);
	set_vbo_texcoord(vbo_texcoord);
	setIbo(ibo);

	glBindVertexArray(Renderable::vao());

	glBindBuffer(GL_ARRAY_BUFFER, Renderable::vbo());
	std::vector<glm::vec3> vertices = {
		//{-0.5,-0.5,0}, {0.5,-0.5,0}, {-0.5,0.5,0}, {0.5,0.5,0}
		{ 0,-1,0 },{ 1,-1,0 },{ 0,0,0 },{ 1,0,0 }
	};
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
		0/*stride*/, 0/*offset*/);


	glBindBuffer(GL_ARRAY_BUFFER, Renderable::vbo_texcoord());
	std::vector<glm::vec2> texcoords = {
			{0,0}, {1,0}, {0,1}, {1,1}
	};
	glBufferData(GL_ARRAY_BUFFER, texcoords.size() * 2 * sizeof(GLfloat), texcoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2/*num elems*/, GL_FLOAT/*elem type*/, GL_FALSE/*normalized*/,
		0/*stride*/, 0/*offset*/);

	std::vector<GLshort> ind = { 0,1,2,3 };
	setIndices(ind);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Renderable::ibo());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices().size()*sizeof(GLshort), indices().data(), GL_STATIC_DRAW);
	glBindVertexArray(0);		
}


Sprite::~Sprite() = default;

void Sprite::render() {
	auto start = std::chrono::high_resolution_clock::now();
	shader()->use();
	

	//glDisable(GL_DEPTH_TEST);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	texture()->bind();

	glm::mat4 model(1);
	glm::vec2 layoutSize = renderer()->entity()->size2D();
	glm::vec2 epos = renderer()->entity()->pos2D();
	glm::vec2 elpos = renderer()->entity()->localPos2D();
	glm::vec2 esize = renderer()->entity()->size2D();
	float fw = 2.0f; // 2 * renderer()->entity()->width() / (float)Engine::instance()->window()->width();
	float fh = 2.0f; // 2 * renderer()->entity()->height() / (float)Engine::instance()->window()->height();
	float fx = -1.0f; // +2 * ((elpos.x + pos().x + pivot().x) / (float)layoutSize.x);
	float fy = 1.0f;
	model = glm::translate(model, glm::vec3(fx, fy, 0));
	model = glm::scale(model, glm::vec3(fw, fh, 1));
	shader()->setUniformMatrix4fv("Model", 1, &model[0][0], false);

	shader()->setUniform1i("Hovered", renderer()->entity()->hovered());
	shader()->setUniform1i("Pressed", renderer()->entity()->pressed());

	glBindVertexArray(vao());
	glBindBuffer(GL_ARRAY_BUFFER, vbo());
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord());
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo());
	drawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, 0);
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	double ms = diff.count() * 1000.0f;
	//std::cout << "Sprite render: " << ms << "ms.\n";
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
