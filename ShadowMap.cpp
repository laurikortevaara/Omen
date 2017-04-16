#include "Omen_OpenGL.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShadowMap.h"
#include "Shader.h"
#include "Engine.h"
#include "system/GraphicsSystem.h"
#include "GL_error.h"

using namespace omen;

ShadowMap::ShadowMap()
{

}

bool ShadowMap::init()
{
	m_shader = new Shader("shaders/shadow_map.glsl");

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &depthTexture);
	// Set the depth map to GraphicsSystem for allowing the mesh renderer to get it later on
	omen::Engine* engine = omen::Engine::instance();
	engine->findSystem<omen::ecs::GraphicsSystem>()->depthMap = depthTexture;

	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthTexture, 0);
	//drawBuffer(GL_NONE); // No color buffer is drawn to.
	//glReadBuffer(GL_NONE); // No read buffer in use
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	return true;
}

void ShadowMap::render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	check_gl_error();
	Engine::instance()->setViewport(0, 0, 4096, 4096);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth buffer

	m_shader->use();
}

void ShadowMap::finish()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	omen::Engine* engine = Engine::instance();
	Engine::instance()->setViewport(0,0,engine->window()->width(), engine->window()->height()); // Render on the whole framebuffer, complete from the lower left corner to the upper right
}