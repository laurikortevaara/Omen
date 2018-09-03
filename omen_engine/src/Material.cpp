//
// Created by Lauri Kortevaara on 27/12/15.
//

#include "Material.h"
#include "math/Vector.h"

using namespace omen;
using namespace omen::math;

Material::Material() :
	m_texture(nullptr),
	m_texture_displacement(nullptr),
	m_texture_normal(nullptr),
	m_texture_shininess_exponent(nullptr),
	m_texture_specular_color(nullptr),
	m_texture_specular_factor(nullptr),
    m_isTwoSided(false),
    m_matcapTexture(nullptr),
    m_diffuse_color(0, 0, 0, 0),
    m_ambient_color(0, 0, 0, 0),
    m_specular_color(0, 0, 0, 0),
    m_emissive_color(0, 0, 0, 0)
{
    // TODO: Define a default color value as constant
    m_diffuse_color = glm::vec4(0.13333f, 0.0f, 0.8f, 1.0f);
}


Material::~Material()
{
	int a = 1;
}