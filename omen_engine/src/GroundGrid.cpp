#include "GroundGrid.h"
#include "MeshProvider.h"

using namespace omen;

struct shader_data_t {
	float fps;
} shader_data[512];

GLuint ssbo = 0;

GroundGrid::GroundGrid()
	: Entity("Ground Grid")
{
	std::unique_ptr<GroundGridRenderer> mr = std::make_unique<GroundGridRenderer>();
	addComponent(std::move(mr));
}

GroundGridRenderer::GroundGridRenderer() :
	Renderer()
{
	m_shader = new Shader("shaders/ground_grid.glsl");
	int a = m_shader->getAttribLocation("vertexIn");

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
		
	glm::vec3 v[4] = { {0,0,0},{ 1,0,0 },{ 0,1,0 },{ 1,1,0 } };

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	glVertexAttribPointer(a, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	
	
	
	Engine::instance()->signal_engine_update.connect([&](float t, float dt) {
		for (int i = 0; i < 511; ++i)
			shader_data[i].fps = shader_data[i+1].fps;
			shader_data[511].fps = Engine::instance()->averageFps();
	});


	GLuint block_index = 0;
	block_index = glGetProgramResourceIndex(m_shader->m_shader_program, GL_SHADER_STORAGE_BLOCK, "ShaderSSBO");

	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_COPY);
	GLuint ssbo_binding_point_index = 3;
	glShaderStorageBlockBinding(m_shader->m_shader_program, block_index, ssbo_binding_point_index);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GroundGridRenderer::render(Shader* shader)
{
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto start = std::chrono::high_resolution_clock::now();
	omen::Window::_size s = Engine::instance()->window()->size();

	m_shader->use();


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &shader_data, sizeof(shader_data));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	glm::mat4 mvp = glm::mat4(1);

	m_shader->setUniformMatrix4fv("ModelViewProjection", 1, glm::value_ptr(mvp), false);
		
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	
	glBindVertexArray(m_vao);
	drawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	double ms = diff.count() * 1000.0f;
	std::cout << "GroundGrid render: " << ms << "ms.\n";
}

void GroundGridRenderer::onAttach(ecs::Entity* e)
{

}

void GroundGridRenderer::onDetach(ecs::Entity* e)
{

}