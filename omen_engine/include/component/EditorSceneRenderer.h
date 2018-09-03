#pragma once

#include "Renderer.h"

namespace omen {
	namespace ecs {

		class EditorSceneRenderer : public Renderer {
		protected:
			virtual void onAttach(ecs::Entity* e);
			virtual void onDetach(ecs::Entity* e);
		public:
			EditorSceneRenderer();
			virtual void render(omen::Shader* shader = nullptr) const;

		protected:
		private:
		};

	}
}