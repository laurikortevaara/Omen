//
// Created by Lauri Kortevaara on 24/11/16.
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
#include "RenderBuffer.h"
#include "Texture.h"
#include "Material.h"
#include "utils.h"
#include "Engine.h"
#include "component/MouseInput.h"

using namespace omen;

RenderBuffer::RenderBuffer() : m_frame_buffer(0), m_colorTexture(0) {
	m_window = Engine::instance()->window();
	createRenderBuffer();
}

bool RenderBuffer::createRenderBuffer() {
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &m_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &m_colorTexture);
	glBindTexture(GL_TEXTURE_2D, m_colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_window->width(), m_window->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_colorTexture, 0);

	glDrawBuffer(GL_BACK);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int w, h;
	int miplevel = 0;
	glBindTexture(GL_TEXTURE_2D, m_colorTexture);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void RenderBuffer::use() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
}

void RenderBuffer::unuse() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}