//
// Created by Lauri Kortevaara(personal) on 27/12/15.
//

#include "Material.h"

using namespace Omen;

Material::Material() :
        m_texture(nullptr),
        m_isTwoSided(false),
        m_matcapTexture(nullptr){
    m_diffuse_color = glm::vec4(0.13333,0,0.8,1);
}
