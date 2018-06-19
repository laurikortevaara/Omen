#pragma once

#include "Renderer.h"

namespace omen {

	namespace ecs {
        class Sprite;

		class SpriteRenderer : public Renderer {
		protected:
			virtual void onAttach(ecs::Entity* e);
			virtual void onDetach(ecs::Entity* e);
		public:
			SpriteRenderer(std::unique_ptr<Sprite> sprite);
			virtual void render(omen::Shader* shader = nullptr) const;

			Sprite* sprite() const { return m_sprite.get(); }

			glm::vec2 pivot() const;
			void setPivot(const glm::vec2& pivot);

		protected:
		private:
			std::unique_ptr<Sprite> m_sprite;
		};

	}
}