//
// Created by Lauri Kortevaara on 25/12/15.
//

#ifndef OMEN_TEXTURE_H
#define OMEN_TEXTURE_H

#include "typedef.h"

#include <iosfwd>
#include <map>
#include <string>

namespace omen {
    class Texture {

        struct _texture_cache_item{
            std::string bitmap_path;
            uint32_t      texture_id;
            uint32_t      width;
            uint32_t      height;
            texture_type  textureTarget;
            bool operator==(const _texture_cache_item& i ){
                return (i.bitmap_path==bitmap_path);
            }
        };
        void loadTexture(const std::string &bitmap_path);

		uint32_t createTexture(); // Returns the created opengl texture ID

		uint32_t read_jpg_file(const char* filename);
		uint32_t read_png_file(const char* filename);

        static std::map<std::string, _texture_cache_item> texture_cache;
		uint32_t m_bitmapType;
        uint32_t m_textureID;
        texture_type m_textureTarget;
        uint32_t m_width;
        uint32_t m_height;

		uint8_t* outData[6];
    private:
        Texture() { };

    public:
        Texture(const std::string &bitmap_path, texture_type textureTarget = omen::TEXTURE_2D );

        uint8_t* raw_data(int pixmap = 0) const { return outData[pixmap]; }
	
        void bind();

        uint32_t width() { return m_width; }

        uint32_t height() { return m_height; }

        uint32_t id(){return m_textureID;}

        void initialize();

        uint32_t m_sampler_state;

        void bindSampler();

        void unbindSampler();
    };
} // namespace omen

#endif //OMEN_TEXTURE_H
