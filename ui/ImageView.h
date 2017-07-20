//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#ifndef OMEN_IMAGEVIEW_H
#define OMEN_IMAGEVIEW_H

#include "../Entity.h"
#include "../component/Sprite.h"
#include "../component/SpriteRenderer.h"
#include "../component/Clickable.h"
#include "View.h"

namespace omen {
	namespace ecs {
		class Sprite;
	}

    namespace ui {
        class ImageView : public View {
        public:
			ImageView(View* parentView, const std::string &name, const std::string &sprite, const glm::vec2& pos = { 0,0 }, const glm::vec2& size = { -1,-1 });
			ImageView(View* parentView, const std::string &name, const std::string &sprite, const glm::vec2& pos, const glm::vec2& size, bool bsprite);
			virtual ~ImageView();

			glm::vec2 pivot() const;
			void setPivot(const glm::vec2& pivot);

			omen::ecs::Sprite* sprite() { getComponent<omen::ecs::SpriteRenderer>()->sprite(); }

        protected:
            virtual void updateLayout();
            virtual void onMeasure(MeasureSpec horintalMeas, MeasureSpec verticalMeas);
        private:
        };
    } // namespace ui
} // namespace omen


#endif //OMEN_IMAGEVIEW_H
