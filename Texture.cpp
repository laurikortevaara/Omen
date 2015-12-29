//
// Created by Lauri Kortevaara(personal) on 25/12/15.
//

#include "Texture.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "GL_error.h"

using namespace Omen;

Texture::Texture(const std::string &bitmap_path) {
    loadTexture(bitmap_path);
}

void Texture::loadTexture(const std::string &bitmap_path) {
    m_textureTarget = GL_TEXTURE_2D;
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int x, y, btm;
    FILE *f = fopen(bitmap_path.c_str(), "r");
    stbi_uc *image = stbi_load_from_file(f, &x, &y, &btm, 0);

    m_width = x;
    m_height = y;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexImage2D(m_textureTarget, 0, btm == 3 ? GL_RGB : GL_RGBA, x, y, 0, btm == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE,
                 (const void *) image);
    check_gl_error();
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind()
{
    glBindTexture(m_textureTarget, m_textureID);
}
