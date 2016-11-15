//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#include "Slider.h"
#include "ImageView.h"
#include "../component/Draggable.h"
#include "../component/Sprite.h"
#include "../component/SpriteRenderer.h"
#include "../Engine.h"
#include "../../component/Sprite.h"
#include "../../component/BorderSprite.h"

using namespace omen;
using namespace ui;

Slider::~Slider() = default;

Slider::Slider(View* parentView, const std::string &name,const std::string &spriteName, const glm::vec2& pos,  int width,  int height) :
        View(parentView, name)
{
	setPos(pos);
	setSize(glm::vec2(width, height));
	std::unique_ptr<Transform> tr = std::make_unique<Transform>();
	tr->pos().x = pos.x;
	tr->pos().y = pos.y;
	tr->setBounds(glm::vec3(0, 0, 0), glm::vec3(width, height, 0));
	addComponent(std::move(tr));

	std::unique_ptr<omen::ecs::Sprite> sprite = nullptr;

	if(name == "SliderFill" )
		sprite = std::make_unique<omen::ecs::BorderSprite>(spriteName, pos, width, height, 10, 10, 3, 3);
	else if (name == "SliderGroove")
		sprite = std::make_unique<omen::ecs::BorderSprite>(spriteName, pos, width, height, 10, 10, 3, 3 );
	else
		sprite = std::make_unique<omen::ecs::Sprite>(spriteName, pos, width, height);

	glm::vec2 knotPos = { 0.0, 0.0 };
	std::unique_ptr<omen::ui::ImageView> knot = std::make_unique<omen::ui::ImageView>(this, "Knot", "textures/slider_knot.png", knotPos);
	std::unique_ptr<omen::ecs::Draggable> dragKnot = std::make_unique<omen::ecs::Draggable>();
	knot->addComponent(std::move(dragKnot));
	addChild(std::move(knot));

	std::unique_ptr<omen::ecs::SpriteRenderer> sr = std::make_unique<omen::ecs::SpriteRenderer>(std::move(sprite));
	addComponent(std::move(sr));
	std::unique_ptr<omen::ecs::Clickable> click = std::make_unique<omen::ecs::Clickable>();
	addComponent(std::move(click));
	    
    getComponent<ecs::Clickable>()->signal_entity_clicked.connect([&](ecs::Entity* e, glm::vec2 pos){
        if(e==this){
            signal_slider_clicked.notify(this,pos);
        }

    });
}

void Slider::updateLayout() {

}

void Slider::onMeasure(float maxwidth, float maxheight) {

}
