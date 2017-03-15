//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#include "Button.h"
#include "../component/Sprite.h"
#include "../component/SpriteRenderer.h"
#include "../Engine.h"
#include "../../component/Sprite.h"
#include "../../component/BorderSprite.h"

using namespace omen;
using namespace ui;

Button::~Button() = default;

Button::Button(View* parentView, const std::string &name,const std::string &spriteName, const glm::vec2& pos,  const glm::vec2& size) :
        ImageView(parentView, name, spriteName, pos, size)
{
	std::unique_ptr<omen::ecs::Clickable> click = std::make_unique<omen::ecs::Clickable>();
	addComponent(std::move(click));
	   
	// Cliced event
    getComponent<ecs::Clickable>()->signal_entity_clicked.connect([&](ecs::Entity* e, glm::vec2 pos, int button){
        if(e==this){
            signal_button_clicked.notify(this,pos);
        }
    });
}

void Button::updateLayout() {

}

void Button::onMeasure(float maxwidth, float maxheight) {

}
