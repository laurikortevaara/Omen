//
// Created by Lauri Kortevaara on 24/11/16.
//
#include "MultipassShader.h"

#include "Texture.h"
#include "Material.h"
#include "utils.h"
#include "Engine.h"

#include "component/MouseInput.h"


#include "GL_error.h"
#include "Omen_OpenGL.h"

#include <boxer/boxer.h>

#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <memory>


using namespace omen;

MultipassShader::MultipassShader() {
}

void MultipassShader::addPass(RenderPass& renderPass) {
	m_renderPasses.push_back(renderPass);
}
void MultipassShader::render() {
	int i_pass = 0;
	for (auto& renderPass : m_renderPasses) {
		renderPass.shader->use();
		check_gl_error();
		renderPass.pass(this, renderPass.shader, i_pass);
		check_gl_error();
		i_pass++;
	}
}