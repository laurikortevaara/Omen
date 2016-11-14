//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#include "Button.h"
#include "../component/Sprite.h"
#include "../component/SpriteRenderer.h"
#include "../Engine.h"
#include "../../component/Sprite.h"

using namespace omen;
using namespace ui;

Button::~Button() = default;

Button::Button(View* parentView, const std::string &name,const std::string &spriteName, const glm::vec2& pos,  int width,  int height) :
        View(parentView, name)
{
	setPos(pos);
	setSize(glm::vec2(width, height));
	std::unique_ptr<Transform> tr = std::make_unique<Transform>();
	tr->pos().x = pos.x;
	tr->pos().y = pos.y;
	tr->setBounds(glm::vec3(0, 0, 0), glm::vec3(width, height, 0));
	addComponent(std::move(tr));

	std::unique_ptr<omen::ecs::Sprite> sprite = std::make_unique<omen::ecs::Sprite>(spriteName, pos, width, height);
	std::unique_ptr<omen::ecs::SpriteRenderer> sr = std::make_unique<omen::ecs::SpriteRenderer>(std::move(sprite));
	addComponent(std::move(sr));
	std::unique_ptr<omen::ecs::Clickable> click = std::make_unique<omen::ecs::Clickable>();
	addComponent(std::move(click));
	    
    getComponent<ecs::Clickable>()->signal_entity_clicked.connect([&](ecs::Entity* e, glm::vec2 pos){
        if(e==this){
            signal_button_clicked.notify(this,pos);
        }

    });
}

void Button::updateLayout() {

}

void Button::onMeasure(float maxwidth, float maxheight) {

}
