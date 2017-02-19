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

Slider::Slider(View* parentView, const std::string &name,const std::string &spriteName, const glm::vec2& pos,  const glm::vec2& size) :
        View(parentView, name, pos, size)
{
	std::unique_ptr<omen::ecs::Sprite> sprite = nullptr;

	//if(name == "SliderFill" )
	//	sprite = std::make_unique<omen::ecs::BorderSprite>(spriteName, pos, width, height, 10, 10, 3, 3);
	//else if (name == "SliderGroove")
	sprite = std::make_unique<omen::ecs::BorderSprite>(spriteName, glm::vec2(0,0), size, 10, 10, 3, 3);
	//else
	//	sprite = std::make_unique<omen::ecs::Sprite>(spriteName, pos, width, height);


	std::unique_ptr<omen::ecs::SpriteRenderer> sr = std::make_unique<omen::ecs::SpriteRenderer>(std::move(sprite));
	addComponent(std::move(sr));
	std::unique_ptr<omen::ecs::Clickable> click = std::make_unique<omen::ecs::Clickable>();
	addComponent(std::move(click));
	    
    getComponent<ecs::Clickable>()->signal_entity_clicked.connect([&](ecs::Entity* e, glm::vec2 pos){
        if(e==this){
            signal_slider_clicked.notify(this,pos);
        }

    });

	glm::vec2 knotPos{0,-2.5};
	std::unique_ptr<omen::ui::ImageView> knot = std::make_unique<omen::ui::ImageView>(this, "Knot", "textures/slider_knot.png", knotPos);
	//knot->setPivot(glm::vec2(0, 4));
	std::unique_ptr<omen::ecs::Draggable> dragKnot = std::make_unique<omen::ecs::Draggable>();
	dragKnot->signal_dragged.connect([this](float value) -> void {
		signal_slider_dragged.notify(this, value);
	});
	knot->addComponent(std::move(dragKnot));
	addChild(std::move(knot));
}

void Slider::updateLayout() {

}

void Slider::onMeasure(float maxwidth, float maxheight) {

}

void Slider::setPos(float pos) {
	Entity* knot = findChild("Knot");
	omen::ecs::Draggable* d = knot->getComponent<omen::ecs::Draggable>();
	//d->setPos(pos);
}

float Slider::pos() const {
	Entity const* knot = findChild_const("Knot");
	omen::ecs::Draggable const* d = getComponent_const<omen::ecs::Draggable>();
	return d->pos();
}