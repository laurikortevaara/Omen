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
            TextView(View *parentView, const std::string& name) : View(parentView, name) { }

        protected:
        private:
            std::string m_text;
        };

    } // namespace ui
} // namespace omen


#endif //OMEN_TEXTVIEW_H
