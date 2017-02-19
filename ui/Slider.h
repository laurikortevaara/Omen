//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#ifndef OMEN_SLIDER_H
#define OMEN_SLIDER_H

#include "../Entity.h"
#include "../component/Sprite.h"
#include "../component/Clickable.h"
#include "View.h"

namespace omen {
	namespace ecs {
		class Sprite;
	}

    namespace ui {
        class Slider : public View {
        public:
            typedef omen::Signal<std::function<void(Slider *, glm::vec2)> > SliderClicked_t;
			SliderClicked_t signal_slider_clicked;

			typedef omen::Signal<std::function<void(Slider *, float)> > SliderDragged_t;
			SliderDragged_t signal_slider_dragged;
        public:
            Slider(View* parentView, const std::string &name, const std::string &sprite, const glm::vec2 &pos, const glm::vec2& size);
			virtual ~Slider();

			void setPos(float pos);
			float pos() const;
        protected:
            virtual void updateLayout();
            virtual void onMeasure(float maxwidth, float maxheight);
        private:
        };
    } // namespace ui
} // namespace omen


#endif //OMEN_SLIDER_H
