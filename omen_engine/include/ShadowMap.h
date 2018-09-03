#pragma once
#include "Omen_OpenGL.h"

namespace omen {
	class Shader;

	class ShadowMap 
	{
	public:
		Shader* m_shader;

		GLuint FramebufferName;
		GLuint depthTexture;
		GLuint depthMatrixID;
		glm::mat4 depthProjectionMatrix;
		glm::mat4 depthViewMatrix;
		glm::mat4 depthModelMatrix;
		glm::mat4 depthMVP;
	public:
		ShadowMap();
		bool init();
		void render();
		void finish();
	};
}