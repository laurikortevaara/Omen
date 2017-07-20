#pragma once

#include "Renderer.h"
#include "../Color.h"

namespace omen {
	class Sprite;
	namespace ecs {

		class PlainColorRenderer : public Renderer {
		protected:
			virtual void onAttach(ecs::Entity* e);
			virtual void onDetach(ecs::Entity* e);
		public:
			PlainColorRenderer(omen::Color& color);
			virtual void render(omen::Shader* shader = nullptr) const;

		protected:
		private:
			Color m_color;
		};

	}
}