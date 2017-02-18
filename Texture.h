//
// Created by Lauri Kortevaara on 25/12/15.
//

#ifndef OMEN_TEXTURE_H
#define OMEN_TEXTURE_H

#ifdef _WIN32
#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#elif __APPLE__
#define GL3_PROTOTYPES
#include <OpenGL/gl3.h>
#else
//#include <GL/glew.h>
#include <glfw/glfw3.h>
#endif
#include <iosfwd>
#include <map>
#include <string>

namespace omen {
    class Texture {

        struct _texture_cache_item{
            std::string bitmap_path;
            GLuint      texture_id;
            GLuint      width;
            GLuint      height;
            GLenum      textureTarget;
            bool operator==(const _texture_cache_item& i ){
                return (i.bitmap_path==bitmap_path);
            }
        };
        void loadTexture(const std::string &bitmap_path);

        static std::map<std::string, _texture_cache_item> texture_cache;
        GLuint m_textureID;
        GLenum m_textureTarget;
        GLuint m_width;
        GLuint m_height;
    private:
        Texture() { };

    public:
        Texture(const std::string &bitmap_path, GLenum textureTarget = GL_TEXTURE_2D );

        void bind();

        GLuint width() { return m_width; }

        GLuint height() { return m_height; }

        GLuint id(){return m_textureID;}

        void initialize();

        GLuint m_sampler_state;

        void bindSampler();

        void unbindSampler();
    };
} // namespace omen

#endif //OMEN_TEXTURE_H
