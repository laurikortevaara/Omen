#pragma once

#include "../ui/View.h"
#include "../ui/LinearLayout.h"
#include "EditorWindow.h"

using namespace omen::ui;

namespace omen {
	namespace editor {
		class GUILayout : public omen::ui::LinearLayout
		{
		public:
			GUILayout(LinearLayout::LayoutDirection dir = LinearLayout::VERTICAL);
			void addWindow(std::unique_ptr<EditorWindow> window);

			virtual bool addChild(std::unique_ptr<View> e);
			virtual void updateLayout();
		protected:

			virtual void onMeasure(MeasureSpec horintalMeas, MeasureSpec verticalMeas);
		private:
			std::unique_ptr<omen::ui::View> m_layout;
		};
	}
}