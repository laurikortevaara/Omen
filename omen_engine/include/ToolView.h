#pragma once

#include "ui/View.h"

namespace omen {
	namespace ui {
		class ToolView : public View {
		public:
			ToolView(const std::string& name, const std::string& sprite, const glm::vec2& pos, const glm::vec2& size);
		protected:
			virtual void updateLayout();
			virtual void onMeasure(float maxwidth, float maxheight) {};
		private:
		};
	}
}