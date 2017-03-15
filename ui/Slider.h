//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
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
            typedef omen::Signal<std::function<void(Slider *, glm::vec2)> > SliderClicked_t;
			SliderClicked_t signal_slider_clicked;

			typedef omen::Signal<std::function<void(Slider *, float)> > SliderDragged_t;
			SliderDragged_t signal_slider_dragged;
        public:
			Slider(View* parentView, const std::string &name, const std::string &sprite, const glm::vec2 &pos, const glm::vec2& size, const glm::vec2& minMax = { 0.0f,1.0f });
			virtual ~Slider();

			void setValue(float value);
			float value() const;

			void setGroovePos(glm::vec2 groovePos) { m_groovePos = groovePos; }
			glm::vec2 groovePos() const { return m_groovePos; }

			void setGrooveSize(glm::vec2 grooveSize) { m_grooveSize = grooveSize; }
			glm::vec2 grooveSize() const { return m_grooveSize; }

			void setCurrentValue(float value);
			void setMinValue(float min) { m_min_value = min; }
			void setMaxValue(float max) { m_max_value = max; }
			float currentValue() const { return m_current_value; }
			float minValue() const { return m_min_value; }
			float maxValue() const { return m_max_value; }

			const std::wstring& label();
			void setLabel(const std::wstring& label);
        protected:
            virtual void updateLayout();
            virtual void onMeasure(float maxwidth, float maxheight);
        private:
			float m_min_value;
			float m_max_value;
			float m_current_value;
			glm::vec2 m_groovePos;
			glm::vec2 m_grooveSize;
			ImageView* m_pKnot;
        };
    } // namespace ui
} // namespace omen


#endif //OMEN_SLIDER_H
