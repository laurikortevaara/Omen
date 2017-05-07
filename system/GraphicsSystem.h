//
// Created by Lauri Kortevaara on 26/12/15.
//

#ifndef OMEN_GRAPHICSSYSTEM_H
#define OMEN_GRAPHICSSYSTEM_H


#include "System.h"
#include "../Omen_OpenGL.h"

namespace omen {
	class Shader;

	namespace ecs {
        class GraphicsSystem : public ecs::System {

        public:
            virtual void render(omen::Shader* pShader = nullptr, int layer=0);

			virtual void shutDown() {};

			int depthMap;

			static long long drawElementsCount;
			static long long drawArraysCount;
			static long long drawElementsInstancedCount;

			static void resetDrawCounters() { drawElementsCount = 0; drawArraysCount = 0; drawElementsInstancedCount = 0; }

			void update(double time, double deltaTime) { drawElementsCount = 0; }

        };
    }
}

inline void drawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
	glDrawElements(mode, count, type, indices);
	omen::ecs::GraphicsSystem::drawElementsCount++;
}

inline void drawArrays(GLenum mode, GLint first, GLsizei count)
{
	glDrawArrays(mode, first, count);
	omen::ecs::GraphicsSystem::drawArraysCount++;
}

inline void drawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount)
{
	glDrawElementsInstanced(mode, count, type, indices, primcount);
	omen::ecs::GraphicsSystem::drawElementsInstancedCount++;
}

#endif //OMEN_GRAPHICSSYSTEM_H
