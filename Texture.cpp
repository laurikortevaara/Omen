//
// Created by Lauri Kortevaara(personal) on 25/12/15.
//

#include "Texture.h"
#include <GLFW/glfw3.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "GL_error.h"
#include "Engine.h"

using namespace omen;

std::map<std::string, Texture::_texture_cache_item> Texture::texture_cache;

Texture::Texture(const std::string &bitmap_path) {
    initialize();
    loadTexture(bitmap_path);
}

void Texture::initialize() {
    m_textureTarget = GL_TEXTURE_2D;
}

void Texture::loadTexture(const std::string &bitmap_path) {
    if(texture_cache.find(bitmap_path)==texture_cache.end())
    {
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        int x, y, btm;
        FILE *f = fopen(bitmap_path.c_str(), "r");
        if(!f)
            return;
        stbi_uc *image = stbi_load_from_file(f, &x, &y, &btm, 0);


        m_width = x;
        m_height = y;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexImage2D(m_textureTarget, 0, btm == 3 ? GL_RGB : GL_RGBA, x, y, 0, btm == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE,
                     (const void *) image);
        glGenerateMipmap(GL_TEXTURE_2D);
        check_gl_error();
        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0);

        texture_cache[bitmap_path] = { "", m_textureID, m_width, m_height, m_textureTarget };
    }
    else
    {
        _texture_cache_item i = texture_cache[bitmap_path];
        m_textureID = i.texture_id;
        m_width = i.width;
        m_height = i.height;
        m_textureTarget = i.textureTarget;
    }
}

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

void Texture::bind()
{
    glBindTexture(m_textureTarget, m_textureID);
}

void Texture::bindSampler(){
    glGenSamplers(1, &m_sampler_state);
    glSamplerParameteri(m_sampler_state, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(m_sampler_state, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(m_sampler_state, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(m_sampler_state, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    float anis = ((int)Engine::instance()->time())%16;
    glSamplerParameterf(m_sampler_state, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
    GLuint texture_unit = 0;
    glBindSampler(texture_unit, m_sampler_state);
}

void Texture::unbindSampler()
{
    glBindSampler(0, 0);
    glDeleteSamplers(1, &m_sampler_state);
}
