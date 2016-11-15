//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#ifndef OMEN_TEXTVIEW_H
#define OMEN_TEXTVIEW_H

#include "View.h"

namespace omen {
    namespace ui {
        class TextView : public View {

        public:
			TextView(View *parentView, const std::string& name);
			const std::wstring& text() const { return m_text; }
			void setText(const std::wstring& text);
        protected:
			virtual void updateLayout() {};
			virtual void onMeasure(float maxwidth, float maxheight) {};
        private:
            std::wstring m_text;
        };

    } // namespace ui
} // namespace omen


#endif //OMEN_TEXTVIEW_H
