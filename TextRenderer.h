//
// Created by Lauri Kortevaara(personal) on 27/12/15.
//

#ifndef OMEN_TEXTRENDERER_H
#define OMEN_TEXTRENDERER_H



#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"

namespace Omen {
    class TextRenderer {
        Shader *m_font_shader;
        FT_Library m_freetype;
        FT_Face m_fontFace;

        bool initializeFreeType();
    public:
        TextRenderer();
        void render_text(const char *text, float fontSize, float x, float y, float sx, float sy, glm::vec4 color);
    };
} // namespace Omen


#endif //OMEN_TEXTRENDERER_H
