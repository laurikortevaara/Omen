//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#include "Button.h"
#include "../component/Sprite.h"
#include "../Engine.h"


using namespace omen;
using namespace ui;

Button::Button(View* parentView, const std::string &name,const std::string &sprite, const glm::vec2& pos,  int width,  int height) :
        View(parentView, name),
        Clickable(this)
{
    addComponent(new ecs::Sprite(sprite,pos, width, height));
    ecs::Clickable::signal_entity_clicked.connect([&](ecs::Entity* e, glm::vec2 pos){
        if(e==this){
            signal_button_clicked.notify(this,pos);
        }

    });
}

void Button::updateLayout() {

}

void Button::onMeasure(float maxwidth, float maxheight) {

}
