//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#ifndef OMEN_IMAGEVIEW_H
#define OMEN_IMAGEVIEW_H

#include "../Entity.h"
#include "../component/Sprite.h"
#include "../component/Clickable.h"
#include "View.h"

namespace omen {
	namespace ecs {
		class Sprite;
	}

    namespace ui {
        class ImageView : public View {
        public:
			ImageView(View* parentView, const std::string &name, const std::string &sprite, const glm::vec2 &pos, int width = -1,
                   int height = -1);
			virtual ~ImageView();
        protected:
            virtual void updateLayout();
            virtual void onMeasure(float maxwidth, float maxheight);
        private:
        };
    } // namespace ui
} // namespace omen


#endif //OMEN_IMAGEVIEW_H
