//
// Created by Lauri Kortevaara on 24/11/16.
//

#ifndef OMEN_MULTIPASS_SHADER_H
#define OMEN_MULTIPASS_SHADER_H

#include <memory>
#include <vector>
#include <string>
#include <functional>

#include "Texture.h"
#include "Material.h"

namespace omen {
	class Shader;

    class MultipassShader {
	public:
		struct RenderPass {
			Shader* shader;
			typedef std::function<void(MultipassShader* ms, Shader* shader, unsigned int pass)> RenderFunc;
			RenderFunc pass;

			RenderPass(Shader* shader, RenderFunc rf):shader(shader),pass(rf){}
		};
		
	public:
		std::vector<RenderPass> m_renderPasses;
    public:
		
		MultipassShader();
		void addPass(RenderPass& renderpass);
		void render();
    };
} // namespace omen

#endif //OMEN_MULTIPASS_SHADER_H
