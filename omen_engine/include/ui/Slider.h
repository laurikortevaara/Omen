//
// Created by Lauri Kortevaara on 04/02/16.
//

#ifndef OMEN_SLIDER_H
#define OMEN_SLIDER_H

#include "../Entity.h"
#include "../component/Sprite.h"
#include "../component/Clickable.h"
#include "View.h"
#include <string>

namespace omen {
	namespace ecs {
		class Sprite;
	}

    namespace ui {

		class ImageView;

        class Slider : public View {
        public:
			enum EasingType
			{
				eLinear,
				eInSine,
				eOutSine,
				eInOutSine,
				eInQuad,
				eOutQuad,
				eInOutQuad,
				eInCubic,
				eOutCubic,
				eInOutCubic,
				eInQuart,
				eOutQuart,
				eInOutQuart,
				eInQuint,
				eOutQuint,
				eInOutQuint,
				eInExpo,
				eOutExpo,
				eInOutExpo,
				eInCirc,
				eOutCirc,
				eInOutCirc,
				eInBack,
				eOutBack,
				eInOutBack,
				eInElastic,
				eOutElastic,
				eInOutElastic,
				eInBounce,
				eOutBounce,
				eInOutBounce,
				eEasingDefault = eLinear
			};

            typedef omen::Signal<std::function<void(Slider *, glm::vec2)> > SliderClicked_t;
			SliderClicked_t signal_slider_clicked;

			typedef omen::Signal<std::function<void(Slider *, float)> > SliderDragged_t;
			SliderDragged_t signal_slider_dragged;
        public:
			Slider(View* parentView, const std::string &name, const std::string &sprite, const glm::vec2 &pos, const glm::vec2& size, const glm::vec2& minMax = { 0.0f,1.0f }, EasingType easingType = eLinear);
			virtual ~Slider();

			void setSliderPosition(float position); // [0,1], set's the slider knot position, as well as set's the value
			float getSliderPosition(float position); // [0,1], set's the slider knot position, as well as set's the value
			void setValue(float value); // [min_value,max_value], set's the value, as well as moves the slider knot to correct position
			float value() const;

			void setGroovePos(glm::vec2 groovePos) { m_groovePos = groovePos; }
			glm::vec2 groovePos() const { return m_groovePos; }

			void setGrooveSize(glm::vec2 grooveSize) { m_grooveSize = grooveSize; }
			glm::vec2 grooveSize() const { return m_grooveSize; }

			//void setCurrentValue(float value);
			void setMinValue(float min) { m_min_value = min; }
			void setMaxValue(float max) { m_max_value = max; }
			float currentValue() const { return m_current_value; }
			float minValue() const { return m_min_value; }
			float maxValue() const { return m_max_value; }

			const std::wstring& label();
			void setLabel(const std::wstring& label);

			void setEasing(EasingType easing) { m_easingType = easing; }
			EasingType easingType() const { return m_easingType; }
        protected:
            virtual void updateLayout();
            virtual void onMeasure(MeasureSpec horintalMeas, MeasureSpec verticalMeas);
        private:
			float m_min_value;
			float m_max_value;
			float m_current_value;
			glm::vec2 m_groovePos;
			glm::vec2 m_grooveSize;
			ImageView* m_pKnot;
			EasingType m_easingType;
        };
    } // namespace ui
} // namespace omen


#endif //OMEN_SLIDER_H
