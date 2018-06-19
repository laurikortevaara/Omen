//
// Created by Lauri Kortevaara on 06/02/16.
//

#ifndef OMEN_TEXTRENDERER_H
#define OMEN_TEXTRENDERER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Renderer.h"

namespace omen {
    namespace ecs {
        class TextRenderer : public Renderer {
		protected:
			virtual void onAttach(ecs::Entity* e) {};
			virtual void onDetach(ecs::Entity* e) {};
        public:
			TextRenderer();
            virtual void render(omen::Shader* shader = nullptr) const;
			void renderText(const std::wstring& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color) const;

			const std::wstring& text() const { return m_text; }
			void setText(const std::wstring& text);

			void invalidate();
			glm::vec2 getTextBounds();
        
        protected:
            virtual void initializeShader();
            virtual void initializeTexture();

        private:
			float m_y_delta;
			float m_textScale;
            FT_Library m_freetype;
            FT_Face m_fontFace;

            bool initializeFreeType();
			bool is_single_line;
			            
            std::wstring m_text;
            GLuint m_texture_id;
        };
    } // namespace ecs
} // namespace omen


#endif //OMEN_TEXTRENDERER_H
