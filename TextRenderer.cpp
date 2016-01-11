//
// Created by Lauri Kortevaara(personal) on 27/12/15.
//

#include <GLFW/glfw3.h>
#include "TextRenderer.h"
#include "GL_error.h"
#include <glm/glm.hpp>
#include <freetype/ftstroke.h>
#include <OpenGL/OpenGL.h>
#include <iostream>


using namespace Omen;


/**
 * CTOR
 */
TextRenderer::TextRenderer() : m_vao(0) {
    initializeFreeType();
    m_font_shader = new Shader("shaders/font_shader.glsl");
}

/**
 * Render given text with given fontsize, position, scale, and color
 */
void TextRenderer::render_text(const char *text, float fontSize, float x, float y, float sx, float sy, glm::vec4 color) {
    m_font_shader->use();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    m_font_shader->setUniform1i("Texture", 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if(!glIsVertexArray(m_vao))
        glGenVertexArrays(1,&m_vao);
    glBindVertexArray(m_vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    GLint attribute_coord = m_font_shader->getAttribLocation("position");
    glEnableVertexAttribArray(attribute_coord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    const char *p;
    FT_GlyphSlot g = m_fontFace->glyph;
    float x_orig = x;

    for (p = text; *p; p++) {
        if(*p=='\n'){
            y-=sx*fontSize+10.0*sx;
            x = x_orig;
            continue;
        }

        FT_Set_Pixel_Sizes(m_fontFace, 0, fontSize);
        m_font_shader->setUniform4fv("FontColor", 1, &color[0]);

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
    check_gl_error();
    glBindVertexArray(0);
}

/**
 * initialize FreeType
 * - load font file
 */
bool TextRenderer::initializeFreeType() {
    if (FT_Init_FreeType(&m_freetype)) {
        std::cerr << "Could not init freetype library" << std::endl;
        return false;
    }

    if (FT_New_Face(m_freetype, "fonts/SourceCodePro-Light.otf", 0, &m_fontFace)) {
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

