#pragma once

#include "Entity.h"
#include "component/Renderer.h"
#include "Shader.h"

namespace omen {
	class GroundGrid : public ecs::Entity
	{
		
	public:
		GroundGrid();
	protected:
	private:
		
	};

	class GroundGridRenderer : public ecs::Renderer
	{
	protected:
		virtual void onAttach(ecs::Entity* e);
		virtual void onDetach(ecs::Entity* e);
	public:
		GroundGridRenderer();
		virtual ~GroundGridRenderer() {};
		virtual void render(Shader* shader);
	private:
		Shader* m_shader;
	};
}