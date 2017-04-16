//
// Created by Lauri Kortevaara(personal) on 24/11/16.
//

#ifndef OMEN_RENDERBUFFER_H
#define OMEN_RENDERBUFFER_H

#include <memory>

#include "Texture.h"
#include "Material.h"

namespace omen {
	class Window;
    class RenderBuffer {
    public:
        RenderBuffer();

        void use() const;
		void unuse() const;

		GLuint texture() const { return m_colorTexture; }

	private:
        GLuint m_frame_buffer;
		GLuint m_colorTexture;

        bool createRenderBuffer();

    };
} // namespace omen

#endif //OMEN_RENDERBUFFER_H
