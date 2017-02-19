//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#ifndef OMEN_SPRITE_H
#define OMEN_SPRITE_H

#include "../Renderable.h"

namespace omen {
    namespace ecs {
        class Sprite : public Renderable {
        protected:
        public:
			Sprite(const std::string &sprite, const glm::vec2& pos, const glm::vec2& size = { -1,-1 });

            virtual ~Sprite();

            virtual void render();

        protected:
            virtual void initializeShader();
            virtual void initializeTexture();
			std::string& sprite() { return m_sprite; }
        private:
            std::string m_sprite;
        };
    } // namespace ecs
} // namespace omen

#endif //OMEN_SPRITE_H
