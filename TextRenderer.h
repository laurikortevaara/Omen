//
// Created by Lauri Kortevaara(personal) on 27/12/15.
//

#ifndef OMEN__TEXTRENDERER_H
#define OMEN__TEXTRENDERER_H



#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"

namespace omen {
    class _TextRenderer {
        Shader *m_font_shader;
        FT_Library m_freetype;
        FT_Face m_fontFace;

        bool initializeFreeType();
    public:
        _TextRenderer();
        void render_text(const wchar_t *text, float fontSize, float x, float y, float sx, float sy, glm::vec4 color);

        GLuint m_vao;
        GLuint m_texture;
        GLuint m_vbo;
    };
} // namespace omen


#endif //OMEN_TEXTRENDERER_H
