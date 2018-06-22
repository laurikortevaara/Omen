//
// Created by Lauri Kortevaara on 04/02/16.
//

#include "ui/Button.h"
#include "Engine.h"
#include "component/MouseInput.h"
#include "component/Sprite.h"
#include "component/SpriteRenderer.h"
#include "component/Sprite.h"
#include "component/BorderSprite.h"

using namespace omen;
using namespace ui;

Button::~Button() = default;

Button::Button(View* parentView, const std::string &name,const std::string &spriteName, const glm::vec2& pos,  const glm::vec2& size) :
        ImageView(parentView, name, spriteName, pos, size)
{
	setGravity(VERTICAL_CENTER);
	/*
	m_texture_normal = new Texture("textures/UI/scroll_arrow_left_idle.png");
	m_texture_hovered = new Texture("textures/UI/scroll_arrow_left_hover.png");
	m_texture_pressed = new Texture("textures/UI/scroll_arrow_left_pressed.png");
	*/
	m_texture_normal = new Texture("textures/UI/button_normal.png");
	m_texture_hovered = new Texture("textures/UI/button_hover.png");
	m_texture_pressed = new Texture("textures/UI/button_pressed.png");
	layoutParams().layoutSizingWidth = LayoutParams::Absolute;
	layoutParams().layoutSizingHeight = LayoutParams::Absolute;
	std::unique_ptr<omen::ecs::Clickable> click = std::make_unique<omen::ecs::Clickable>();
	addComponent(std::move(click));
	   
	// Cliced event
    getComponent<ecs::Clickable>()->signal_entity_mouse_clicked.connect(this,[&](ecs::Entity* e, glm::vec2 pos, int button){
        if(e==this){
            signal_button_clicked.notify(this,pos);
        }
    });

	getComponent<ecs::Clickable>()->signal_entity_mouse_pressed.connect(this, [&](ecs::Entity* e, glm::vec2 pos, int button) {
		if (e == this) {
			getComponent<ecs::SpriteRenderer>()->sprite()->setTexture(m_texture_pressed);
		}
	});

	getComponent<ecs::Clickable>()->signal_entity_mouse_released.connect(this, [&](ecs::Entity* e, glm::vec2 pos, int button) {
		if(e == this)
			getComponent<ecs::SpriteRenderer>()->sprite()->setTexture(m_texture_hovered);
		else
			getComponent<ecs::SpriteRenderer>()->sprite()->setTexture(m_texture_normal);
	});

	signal_entered.connect(this, [&](ecs::Entity* e, glm::vec2 pos) {
		if (e == this) {
			getComponent<ecs::SpriteRenderer>()->sprite()->setTexture(m_texture_hovered);
		}		
	});

	signal_exited.connect(this, [&](ecs::Entity* e, glm::vec2 pos) {
		if (e == this) {
			getComponent<ecs::SpriteRenderer>()->sprite()->setTexture(m_texture_normal);
		}
	});
}

void Button::updateLayout() {
	View::updateLayout();
}

void Button::onMeasure(MeasureSpec horintalMeas, MeasureSpec verticalMeas)
{
	setMeasuredSize(getComponent<omen::ecs::SpriteRenderer>()->sprite()->width(), getComponent<omen::ecs::SpriteRenderer>()->sprite()->height());
}
