//
// Created by Lauri Kortevaara(personal) on 25/12/15.
//

#ifndef OMEN_TEXTURE_H
#define OMEN_TEXTURE_H

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <iosfwd>
#include <map>
#include <string>

namespace Omen {
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
        Texture(const std::string &bitmap_path);

        void bind();

        GLuint width() { return m_width; }

        GLuint height() { return m_height; }

        GLuint id();

        void initialize();
    };
} // namespace Omen

#endif //OMEN_TEXTURE_H
