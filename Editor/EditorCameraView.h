#pragma once

#include "EditorWindow.h"

namespace omen
{
	namespace editor {
		class EditorCameraView : public EditorWindow
		{
		public:
			EditorCameraView();
			virtual ~EditorCameraView();
			void initialize();
		protected:
		private:
		};
	}
}