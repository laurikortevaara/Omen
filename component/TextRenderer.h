//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#ifndef OMEN_TEXTRENDERER_H
#define OMEN_TEXTRENDERER_H

#include <freetype/freetype.h>
#include "Renderable.h"

namespace omen {
    namespace ecs {
        class TextRenderer : public Renderable {
        public:
            virtual void render();
        protected:
        protected:
            virtual void initializeShader();
            virtual void initializeTexture();
        private:
            FT_Library m_freetype;
            FT_Face m_fontFace;

            bool initializeFreeType();

            void render_text(const wchar_t *text, float fontSize, float x, float y, float sx, float sy, glm::vec4 color);
            std::string m_text;
            GLuint m_texture_id;
        };
    } // namespace ecs
} // namespace omen


#endif //OMEN_TEXTRENDERER_H