#pragma once
#include <string>
#include "../ui/View.h"

namespace omen
{
	namespace editor {
		class EditorWindow : public ui::View
		{
		public:
			EditorWindow(const std::string& title);

		protected:
			virtual void updateLayout();
			virtual void onMeasure(ui::MeasureSpec horintalMeas, ui::MeasureSpec verticalMeas);
		private:
		};
	}
}