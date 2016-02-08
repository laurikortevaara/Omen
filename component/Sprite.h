//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#ifndef OMEN_SPRITE_H
#define OMEN_SPRITE_H

#include "Renderable.h"

namespace omen {
    namespace ecs {
        class Sprite : public Renderable {
        protected:
            virtual void onAttach(ecs::Entity* e);
            virtual void onDetach(ecs::Entity* e);

        public:
            Sprite(const std::string &sprite,const glm::vec2& pos, float width=-1, float height=-1);

            virtual ~Sprite();

            virtual void render();

        protected:
            virtual void initializeShader();
            virtual void initializeTexture();
        private:
            std::string m_sprite;
        };
    } // namespace ecs
} // namespace omen

#endif //OMEN_SPRITE_H
