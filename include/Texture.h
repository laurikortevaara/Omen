//
// Created by Lauri Kortevaara on 25/12/15.
//

#ifndef OMEN_TEXTURE_H
#define OMEN_TEXTURE_H

#include "Omen_OpenGL.h"

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

		GLuint createTexture(); // Returns the created opengl texture ID

		GLuint read_jpg_file(const char* filename);
		GLuint read_png_file(const char* filename);

        static std::map<std::string, _texture_cache_item> texture_cache;
		GLuint m_bitmapType;
        GLuint m_textureID;
        GLenum m_textureTarget;
        GLuint m_width;
        GLuint m_height;

		BYTE* outData[6];
    private:
        Texture() { };

    public:
        Texture(const std::string &bitmap_path, GLenum textureTarget = GL_TEXTURE_2D );

		BYTE* raw_data(int pixmap = 0) const { return outData[pixmap]; }
	
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
