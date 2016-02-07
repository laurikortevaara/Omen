//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#ifndef OMEN_BUTTON_H
#define OMEN_BUTTON_H

#include "../Entity.h"
#include "../component/Sprite.h"
#include "../component/Clickable.h"
#include "View.h"

namespace omen {
    namespace ui {
        class Button : public View, public ecs::Clickable {
        public:
            typedef omen::Signal<std::function<void(Button *, glm::vec2)> > ButtonClicked_t;

            ButtonClicked_t signal_button_clicked;
        public:
            Button(View* parentView, const std::string &name, const std::string &sprite, const glm::vec2 &pos, int width = -1,
                   int height = -1);
        protected:
            virtual void updateLayout();
            virtual void onMeasure(float maxwidth, float maxheight);
        private:

        };
    } // namespace ui
} // namespace omen


#endif //OMEN_BUTTON_H