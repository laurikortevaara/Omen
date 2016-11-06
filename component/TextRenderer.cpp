//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Omen_OpenGL.h"
#include "TextRenderer.h"
#include "../GL_error.h"
#include <glm/glm.hpp>
#include <iostream>

using namespace omen;
using namespace ecs;

void TextRenderer::render() {
	return;
    initializeFreeType();
    glGenTextures(1, &m_texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if(!glIsVertexArray(m_vao))
        glGenVertexArrays(1,&m_vao);
}

bool TextRenderer::initializeFreeType() {
    if (FT_Init_FreeType(&m_freetype)) {
        std::cerr << "Could not init freetype library" << std::endl;
        return false;
    }

    if (FT_New_Face(m_freetype, "fonts/SourceCodePro-Regular.otf", 0, &m_fontFace)) {
        std::cerr << "Could not open font" << std::endl;
        return false;
    }
    FT_Set_Pixel_Sizes(m_fontFace, 0, 48);

    if (FT_Load_Char(m_fontFace, 'X', FT_LOAD_RENDER)) {
        std::cerr << "Could not load character 'X'" << std::endl;
        return false;
    }
    return true;

}

void TextRenderer::render_text(const wchar_t *text, float fontSize, float x, float y, float sx, float sy,
                               glm::vec4 color) {
    shader()->use();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);

    shader()->setUniform1i("Texture", 0);

    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    GLint attribute_coord = shader()->getAttribLocation("position");
    glEnableVertexAttribArray(attribute_coord);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    const wchar_t *p;
    FT_GlyphSlot g = m_fontFace->glyph;
    float x_orig = x;

    for (p = text; *p; p++) {
        if(*p=='\n'){
            y-=static_cast<omen::floatprec>(sx*fontSize+10.0*sx);
            x = x_orig;
            continue;
        }

        FT_Set_Pixel_Sizes(m_fontFace, 0, static_cast<FT_UInt>(fontSize));
        shader()->setUniform4fv("FontColor", 1, &color[0]);

        if (FT_Load_Char(m_fontFace, *p, FT_LOAD_RENDER))
            continue;

        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                g->bitmap.width,
                g->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                g->bitmap.buffer
        );

        float x2 = x + g->bitmap_left * sx;
        float y2 = -y - g->bitmap_top * sy;
        float w = g->bitmap.width * sx;
        float h = g->bitmap.rows * sy;

        GLfloat box[4][4] = {
                {x2 + w, -y2,     1, 0},
                {x2,     -y2,     0, 0},
                {x2 + w, -y2 - h, 1, 1},
                {x2,     -y2 - h, 0, 1},
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        x += (g->advance.x >> 6) * sx;
        y += (g->advance.y >> 6) * sy;
    }
    //check_gl_error();
    glBindVertexArray(0);
}

void TextRenderer::initializeShader() {
    setShader(std::make_shared<Shader>("shaders/font_shader.glsl"));
}

void TextRenderer::initializeTexture() {
    setTexture(nullptr);
}
