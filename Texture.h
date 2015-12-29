//
// Created by Lauri Kortevaara(personal) on 25/12/15.
//

#ifndef OMEN_TEXTURE_H
#define OMEN_TEXTURE_H

#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <iosfwd>

namespace Omen {
    class Texture {

        void loadTexture(const std::string &bitmap_path);

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
    };
} // namespace Omen

#endif //OMEN_TEXTURE_H
