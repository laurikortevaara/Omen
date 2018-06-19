//
// Created by Lauri Kortevaara on 27/12/15.
//

#include "Material.h"

using namespace omen;

Material::Material() :
	m_texture(nullptr),
	m_texture_displacement(nullptr),
	m_texture_normal(nullptr),
	m_texture_shininess_exponent(nullptr),
	m_texture_specular_color(nullptr),
	m_texture_specular_factor(nullptr),
    m_isTwoSided(false),
    m_matcapTexture(nullptr){
    m_diffuse_color = glm::vec4(0.13333,0,0.8,1);
}


Material::~Material()
{
	int a = 1;
}