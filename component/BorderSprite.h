//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#ifndef OMEN_BORDER_SPRITE_H
#define OMEN_BORDER_SPRITE_H

#include "Sprite.h"

namespace omen {
    namespace ecs {
        class BorderSprite : public Sprite {
        protected:

        public:
            BorderSprite(const std::string &sprite,const glm::vec2& pos, float width, float height, int left, int right, int top, int bottom );

            virtual ~BorderSprite();

            virtual void render();

        protected:
            virtual void initializeShader();
            virtual void initializeTexture();
        private:
  			int left, right, top, bottom;
        };
    } // namespace ecs
} // namespace omen

#endif //OMEN_BORDER_SPRITE_H
