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
            Layout(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size);
			virtual void updateLayout() = 0;
			virtual void onMeasure(float maxwidth, float maxheight) = 0;
        private:

        };
    } // namespace ui
} // namespace omen


#endif //OMEN_LAYOUT_H
