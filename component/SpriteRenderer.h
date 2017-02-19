#pragma once

#include "Renderer.h"

namespace omen {
	class Sprite;
	namespace ecs {

		class SpriteRenderer : public Renderer {
		protected:
			virtual void onAttach(ecs::Entity* e);
			virtual void onDetach(ecs::Entity* e);
		public:
			SpriteRenderer(std::unique_ptr<Sprite> sprite);
			virtual void render(Shader* shader = nullptr);

			Sprite* sprite() const { return m_sprite.get(); }

			glm::vec2 pivot() const;
			void setPivot(const glm::vec2& pivot);

		protected:
		private:
			std::unique_ptr<Sprite> m_sprite;
		};

	}
}