//
// Created by Lauri Kortevaara on 25/12/15.
//

#include "Texture.h"
#include <GLFW/glfw3.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "thirdparty/lodepng/lodepng.h"
#include "thirdparty/libpng/png.h"

#include "GL_error.h"
#include "Engine.h"

using namespace omen;

std::map<std::string, Texture::_texture_cache_item> Texture::texture_cache;

Texture::Texture(const std::string &bitmap_path, GLenum textureTarget) 
	: m_textureTarget(textureTarget)
{
	initialize();
	loadTexture(bitmap_path);
}

void Texture::initialize() {
	for (int i = 0; i < 6; ++i)
		outData[i] = nullptr;
}

GLuint Texture::createTexture()
{
	// Generate the OpenGL texture object
	GLuint textureID;
	glGenTextures(1, &textureID);
	check_gl_error();
	glBindTexture(m_textureTarget, textureID);

	switch (m_textureTarget)
	{
		case GL_TEXTURE_2D: 
		{
			glTexImage2D(m_textureTarget, 0, m_bitmapType, m_width, m_height, 0, m_bitmapType, GL_UNSIGNED_BYTE, outData[0]);

			//glGenerateMipmap(GL_TEXTURE_2D);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		}
		case GL_TEXTURE_CUBE_MAP:
		{
			for(int i=0; i < 6; ++i)
			{
				// Width / 4
				// height / 3
				int bpp = 3;
				int w = width() / 4;
				int h = height() / 3;
				BYTE* raw_data = (BYTE*)malloc(w * h * bpp);
				int y_section = 0;
				int x_section = 0;

				enum eSections { Right, Left, Top, Bottom, Front, Back };

				switch(i)
				{
					// Right, Left, Top, Bottm, Front, Back
					
					case Right:
						y_section = 1;
						x_section = 2;
					break;
					case Left:
						y_section = 1;
						x_section = 0;
						break;
					case Top:
						y_section = 2;
						x_section = 1;
						break;
					case Bottom:
						y_section = 0;
						x_section = 1;
						break;
					case Front:
						y_section = 1;
						x_section = 1;
						break;
					case Back:
						y_section = 1;
						x_section = 3;
						break;
					
				}
				for (int y = 0; y < h; ++y)
				{
					unsigned long offset = (y_section*h + y)*width()*bpp + x_section*w*bpp;
					BYTE* ptr = this->outData[0] + offset;
					memcpy(raw_data+(h-y-1)*w*bpp, ptr, w*bpp);
				}

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_data);
				//delete raw_data;

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

				/*
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
				*/
			}
			break;
		}
	}
	
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	return textureID;
}

void Texture::loadTexture(const std::string &bitmap_path) {
	Engine::getTextureMemoryInfo();

	if (texture_cache.find(bitmap_path) != texture_cache.end())
	{
		_texture_cache_item i = texture_cache[bitmap_path];
		m_textureID = i.texture_id;
		m_width = i.width;
		m_height = i.height;
		m_textureTarget = i.textureTarget;
	}
	else
	{
		if (bitmap_path.find(".png") != std::string::npos)
		{
			read_png_file(const_cast<char*>(bitmap_path.c_str()));
		}
		else
		if (bitmap_path.find(".jpg") != std::string::npos || bitmap_path.find(".tga") != std::string::npos)
		{
			read_jpg_file(const_cast<char*>(bitmap_path.c_str()));
		}

		// Create the actual texture from the loaded raw data
		m_textureID = createTexture();

		// Store the bitmap into texture cache
		texture_cache[bitmap_path] = { bitmap_path, m_textureID, m_width, m_height, m_textureTarget };
	}
}

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

void Texture::bind()
{
	glBindTexture(m_textureTarget, m_textureID);
}

void Texture::bindSampler() {
	glGenSamplers(1, &m_sampler_state);
	glSamplerParameteri(m_sampler_state, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_sampler_state, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_sampler_state, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(m_sampler_state, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	float anis = static_cast<float>(((int)Engine::instance()->time()) % 16);
	glSamplerParameterf(m_sampler_state, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	GLuint texture_unit = 0;
	glBindSampler(texture_unit, m_sampler_state);
}

void Texture::unbindSampler()
{
	glBindSampler(0, 0);
	glDeleteSamplers(1, &m_sampler_state);
}


/**
	Read a png file into memory
**/
GLuint Texture::read_png_file(const char* file_name)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytepp row_pointers;

	unsigned int sig_read = 0;
	png_uint_32 width, height;
	png_int_32 bit_depth;
	png_int_32 color_type, interlace_type;
	FILE *fp;

	if ((fopen_s(&fp, file_name, "rb")) != 0)
		return false;

	/* Create and initialize the png_struct
	* with the desired error handler
	* functions.  If you want to use the
	* default stderr and longjump method,
	* you can supply NULL for the last
	* three parameters.  We also supply the
	* the compiler header file version, so
	* that we know if the application
	* was compiled with a compatible version
	* of the library.  REQUIRED
	*/
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(fp);
		return false;
	}

	/* Allocate/initialize the memory
	* for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return false;
	}

	/* Set error handling if you are
	* using the setjmp/longjmp method
	* (this is the normal method of
	* doing things with libpng).
	* REQUIRED unless you  set up
	* your own error handlers in
	* the png_create_read_struct()
	* earlier.
	*/
	if (setjmp(png_jmpbuf(png_ptr))) {
		/* Free all of the memory associated
		* with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		/* If we get here, we had a
		* problem reading the file */
		return false;
	}

	/* Set up the output control if
	* you are using standard C streams */
	png_init_io(png_ptr, fp);

	/* If we have already
	* read some of the signature */
	png_set_sig_bytes(png_ptr, sig_read);

	/*
	* If you have enough memory to read
	* in the entire image at once, and
	* you need to specify only
	* transforms that can be controlled
	* with one of the PNG_TRANSFORM_*
	* bits (this presently excludes
	* dithering, filling, setting
	* background, and doing gamma
	* adjustment), then you can read the
	* entire image (including pixels)
	* into the info structure with this
	* call
	*
	* PNG_TRANSFORM_STRIP_16 |
	* PNG_TRANSFORM_PACKING  forces 8 bit
	* PNG_TRANSFORM_EXPAND forces to
	*  expand a palette into RGB
	*/
	
	/*png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);*/

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
	
	m_width = width;
	m_height = height;

	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	row_pointers = png_get_rows(png_ptr, info_ptr);
	float bpp = static_cast<float>(row_bytes / width);

	if (outData[0] != nullptr)
		delete outData[0];

	outData[0] = (unsigned char*)malloc(row_bytes * height);

	for (size_t i = 0; i < height; i++) {
		// note that png is ordered top to
		// bottom, but OpenGL expect it bottom to top
		// so the order or swapped
		memcpy(outData[0] + (row_bytes * (height - 1 - i)), row_pointers[i], row_bytes);
	}

	int gray = PNG_COLOR_TYPE_GRAY;
	int palette = PNG_COLOR_TYPE_PALETTE;
	int rgb = PNG_COLOR_TYPE_RGB;
	int rgba = PNG_COLOR_TYPE_RGB_ALPHA;
	int gray_alpha = PNG_COLOR_TYPE_GRAY_ALPHA;

	m_bitmapType = GL_RGBA;
	int _bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	int _color_type = png_get_color_type(png_ptr, info_ptr);
	switch (_color_type)
	{
	case PNG_COLOR_TYPE_GRAY:
		m_bitmapType = GL_ALPHA;
		break;
	case PNG_COLOR_TYPE_RGB:
		m_bitmapType = GL_RGB;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		m_bitmapType = GL_RGBA;
		break;
	default:
		abort();
	}

	/* Clean up after the read,
	* and free any memory allocated */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	/* Close the file */
	fclose(fp);
	return 0;
}

/**
	Read a jpg file into memory
**/
GLuint Texture::read_jpg_file(const char* fileName)
{
	int x, y, btm;
	FILE *f = nullptr;
	errno_t ferror = fopen_s(&f, fileName, "r");
	if (!f || ferror != 0)
		return 0;
	int w;
	int h;
	int comp;
	stbi_info_from_file(f, &w, &h, &comp);

	fclose(f);
	stbi_uc *image = stbi_load(fileName, &x, &y, &btm, 0);
	if (image == nullptr)
	{
		std::string str = stbi_failure_reason();
		if (!str.empty())
			std::cout << str << std::endl;
	}
		
	m_width = x;
	m_height = y;
	outData[0] = new unsigned char[x*y*btm];
	if(strstr(".jpg",fileName) != nullptr )
		for (size_t i = 0; i < m_height; i++) {
		// note that png is ordered top to
		// bottom, but OpenGL expect it bottom to top
		// so the order or swapped
		memcpy(outData[0] + (btm*x * (i)), (BYTE*)(image)+(i*btm*x), btm*x);
	}
	else
		for (size_t i = 0; i < m_height; i++)
			memcpy(outData[0] + (btm*x * (y-1-i)), (BYTE*)(image)+(i*btm*x), btm*x);
	m_bitmapType = btm == 3 ? GL_RGB : GL_RGBA;
	stbi_image_free(image);
	fclose(f);

	return 0;
}