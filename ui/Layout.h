//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#ifndef OMEN_LAYOUT_H
#define OMEN_LAYOUT_H


#include "View.h"

namespace omen {
    namespace ui {
        class Layout : public View {
        public:

        protected:
            Layout(View *parentView, const std::string& name);
            virtual void updateLayout() = 0;
        private:

        };
    } // namespace ui
} // namespace omen


#endif //OMEN_LAYOUT_H
