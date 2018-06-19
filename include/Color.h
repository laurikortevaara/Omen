#pragma once

#include <string>
#include <sstream>
#include <glm/glm.hpp>

namespace omen
{
	class Color
	{
	public:
		Color() : m_color(1.0f, 1.0f, 1.0f, 1.0f) {}
		Color(const Color& color) : m_color(color.m_color) {};

		Color(const glm::vec4& color) : m_color(color) {}
		explicit Color(float r, float g, float b, float a = 1.0f) : m_color(r, g, b, a) {}
		explicit Color(int r, int g, int b, int a = 255) : m_color((r+1)/float(256.0f), (g+1) / float(256.0f), (b+1) / float(256.0f), (a+1) / float(256.0f)) {} // input params 0..255

		operator glm::vec4() const { return m_color; }
		
		Color& operator=(const Color& color) { m_color = color.m_color; return *this; }

		Color(const std::string& color_rgba) {
			float r=0.0f, g=0.0f, b=0.0f, a = 1.0f;
			std::string color = color_rgba;
			if(color_rgba.find("#") != std::string::npos){
				color = color.substr(color.find("#")+1);
			}
			if (color.length() < 6)
				Color();
			if (color.length() < 8)
			{ 
				unsigned int color_hex;
				std::stringstream ss(color);
				ss >> std::hex >> color_hex;
				r = (((color_hex / 0x10000) % 0x100) + 1) / 256.0f;
				g = (((color_hex / 0x100) % 0x100) + 1) / 256.0f;
				b = ((color_hex % 0x100) + 1) / 256.0f;
			}
			else
			{
				unsigned int color_hex;
				std::stringstream ss(color);
				ss >> std::hex >> color_hex;
				r = ((color_hex / 0x1000000)+1) / 256.0f;
				g = (((color_hex / 0x10000)%0x100)+1) / 256.0f;
				b = (((color_hex / 0x100) % 0x100)+1) / 256.0f;
				a = ((color_hex % 0x100)+1) / 256.0f;
			}
			m_color = glm::vec4(r, g, b, a);
		}
	protected:
	private:
		glm::vec4 m_color; // r,g,b,a [0..1]
	};
}
