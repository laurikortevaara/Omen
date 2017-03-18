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
}

int x, y;

int width, height;
png_byte color_type;
png_byte bit_depth;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep * row_pointers;

GLuint read_png_file(char* file_name, int* w, int* h)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	int color_type, interlace_type;
	FILE *fp;

	if ((fp = fopen(file_name, "rb")) == NULL)
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
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	png_uint_32 width, height;
	int bit_depth;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		&interlace_type, NULL, NULL);
	*w = width;
	*h = height;

	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	BYTE* outData = (unsigned char*)malloc(row_bytes * height);

	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

	for (int i = 0; i < height; i++) {
		// note that png is ordered top to
		// bottom, but OpenGL expect it bottom to top
		// so the order or swapped
		memcpy(outData + (row_bytes * (height - 1 - i)), row_pointers[i], row_bytes);
	}

	int gray = PNG_COLOR_TYPE_GRAY;
	int palette = PNG_COLOR_TYPE_PALETTE;
	int rgb = PNG_COLOR_TYPE_RGB;
	int rgba = PNG_COLOR_TYPE_RGB_ALPHA;
	int gray_alpha = PNG_COLOR_TYPE_GRAY_ALPHA;

	int bitmap_type = GL_RGBA;
	int _bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	int _color_type = png_get_color_type(png_ptr, info_ptr);
	switch (_color_type) 
	{
	case PNG_COLOR_TYPE_GRAY:
		bitmap_type = GL_ALPHA;
		break;
	case PNG_COLOR_TYPE_RGB:
		bitmap_type = GL_RGB;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		bitmap_type = GL_RGBA;
		break;
	default:
		abort();
	}

	/* Clean up after the read,
	* and free any memory allocated */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	/* Close the file */
	fclose(fp);
		
	// Generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, bitmap_type, width, height, 0, bitmap_type, GL_UNSIGNED_BYTE, outData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	*w = width;
	*h = height;

	fclose(fp);

	return texture;
}


GLuint png_texture_load(const char * file_name, int * width, int * height)
{
	png_byte header[8];

	FILE *fp = nullptr;
	errno_t ferror = fopen_s(&fp, file_name, "rb");
	if (fp == 0 || ferror != 0)
	{
		perror(file_name);
		return 0;
	}

	// read the header
	fread(header, 1, 8, fp);

	if (png_sig_cmp(header, 0, 8))
	{
		fprintf(stderr, "error: %s is not a PNG.\n", file_name);
		fclose(fp);
		return 0;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fprintf(stderr, "error: png_create_read_struct returned 0.\n");
		fclose(fp);
		return 0;
	}

	// create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return 0;
	}

	// create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return 0;
	}

	// the code in this if statement gets called if libpng encounters an error
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "error from libpng\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return 0;
	}

	// init png reading
	png_init_io(png_ptr, fp);

	// let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	// variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 temp_width, temp_height;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
		NULL, NULL, NULL);

	if (width) { *width = temp_width; }
	if (height) { *height = temp_height; }

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// glTexImage2d requires rows to be 4-byte aligned
	rowbytes += 3 - ((rowbytes - 1) % 4);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte * image_data;
	image_data = static_cast<png_byte*>(malloc(rowbytes * temp_height * sizeof(png_byte) + 15));
	if (image_data == NULL)
	{
		fprintf(stderr, "error: could not allocate memory for PNG image data\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return 0;
	}

	// row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep * row_pointers = static_cast<png_bytep*>(malloc(temp_height * sizeof(png_bytep)));
	if (row_pointers == NULL)
	{
		fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		free(image_data);
		fclose(fp);
		return 0;
	}

	// set the individual row_pointers to point at the correct offsets of image_data
	int i;
	for (i = temp_height - 1; i >= 0; i--)
	{
		row_pointers[i] = image_data + i * rowbytes;
	}

	// read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	unsigned long color = 0x000000;
	for (int h = 0; h < *height; ++h)
		for (int w = 0; w < *width; ++w)
			if (w < 50 && h < 50)
				memcpy(&image_data[h*(*height) + w], &color, 4);
	// Generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, temp_width, temp_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	// clean up
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(image_data);
	free(row_pointers);
	fclose(fp);
	return texture;
}

void Texture::loadTexture(const std::string &bitmap_path) {
	Engine::getTextureMemoryInfo();
	if (bitmap_path.find(".png") != std::string::npos)
	{

		int w, h;
		m_textureID = read_png_file(const_cast<char*>(bitmap_path.c_str()), &w, &h);//png_texture_load(bitmap_path.c_str(), &w, &h);
		m_width = w;
		m_height = h;
		texture_cache[bitmap_path] = { "", m_textureID, m_width, m_height, m_textureTarget };
	}
	else
		if (texture_cache.find(bitmap_path) == texture_cache.end())
		{
			glGenTextures(1, &m_textureID);
			glBindTexture(GL_TEXTURE_2D, m_textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			int x, y, btm;
			FILE *f = nullptr;
			errno_t ferror = fopen_s(&f, bitmap_path.c_str(), "r");
			if (!f || ferror != 0)
				return;
			fclose(f);
			stbi_uc *image = stbi_load(bitmap_path.c_str(), &x, &y, &btm, 0);
			if (image == nullptr)
			{
				std::string str = stbi_failure_reason();
				if (!str.empty())
					std::cout << str << std::endl;
			}

			m_width = x;
			m_height = y;
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			unsigned char* buf = new unsigned char[x*y*btm];
			memcpy(buf, image, x*y*btm*sizeof(unsigned char));
			glTexImage2D(m_textureTarget, 0, btm == 3 ? GL_RGB : GL_RGBA, x, y, 0, btm == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE,
				(const void *)buf);
			glGenerateMipmap(GL_TEXTURE_2D);
			check_gl_error();
			stbi_image_free(image);
			glBindTexture(GL_TEXTURE_2D, 0);

			texture_cache[bitmap_path] = { "", m_textureID, m_width, m_height, m_textureTarget };
			delete buf;
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
	if (!glIsTexture(m_textureID))
	{
		int a = 1;
	}
	else
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
