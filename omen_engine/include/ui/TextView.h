//
// Created by Lauri Kortevaara on 06/02/16.
//

#ifndef OMEN_TEXTVIEW_H
#define OMEN_TEXTVIEW_H

#include "View.h"

namespace omen {
    namespace ui {
        class TextView : public View {

        public:
			TextView(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size);
			const std::wstring& text() const { return m_text; }
			void setText(const std::wstring& text);

			virtual void setWidth(float width);
			virtual void setHeight(float height);
			
        protected:
			virtual void onParentChanged();
			virtual void updateLayout();
			virtual void onMeasure(MeasureSpec horintalMeas, MeasureSpec verticalMeas) {};
        private:
            std::wstring m_text;
        };

    } // namespace ui
} // namespace omen


#endif //OMEN_TEXTVIEW_H
