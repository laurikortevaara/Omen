//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#ifndef OMEN_FRAMELAYOUT_H
#define OMEN_FRAMELAYOUT_H


#include "Layout.h"

namespace omen {
    namespace ui {
        class FrameLayout : public Layout {
        public:
            FrameLayout(View *parentView, const std::string& name);
        protected:
            virtual void updateLayout();
        private:
        };

    } // namespace ui
} // namespace omen

#endif //OMEN_FRAMELAYOUT_H
