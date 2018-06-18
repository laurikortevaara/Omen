//
// Created by Lauri Kortevaara on 04/02/16.
//

#include "ImageView.h"

#include "../Engine.h"
#include "../component/Sprite.h"
#include "../component/SpriteRenderer.h"
#include "../component/BorderSprite.h"

using namespace omen;
using namespace ui;

ImageView::~ImageView() = default;

ImageView::ImageView(View* parentView, const std::string &name,const std::string &spriteName, const glm::vec2& pos, const glm::vec2& size) :
        View(parentView, name, pos, size)
{
	std::unique_ptr<omen::ecs::Sprite> sprite = nullptr;
	sprite = std::make_unique<omen::ecs::Sprite>(spriteName, pos, size);
	
	if (width() == -1 && height() == -1)
	{
		setSize2D(sprite->size());
		tr()->setBounds(glm::vec3(0, 0, 0), glm::vec3(sprite->width(), sprite->height(), 0));
	}

	std::unique_ptr<omen::ecs::SpriteRenderer> sr = std::make_unique<omen::ecs::SpriteRenderer>(std::move(sprite));
	addComponent(std::move(sr));
}


ImageView::ImageView(View* parentView, const std::string &name, const std::string &spriteName, const glm::vec2& pos, const glm::vec2& size, bool bsprite) :
	View(parentView, name, pos, size)
{
	std::unique_ptr<omen::ecs::Sprite> sprite = nullptr;
	sprite = std::make_unique<omen::ecs::BorderSprite>(spriteName, pos, size, 2, 2, 87, 2);

	if (width() == -1 && height() == -1)
	{
		setSize2D(sprite->size());
		tr()->setBounds(glm::vec3(0, 0, 0), glm::vec3(sprite->width(), sprite->height(), 0));
	}

	std::unique_ptr<omen::ecs::SpriteRenderer> sr = std::make_unique<omen::ecs::SpriteRenderer>(std::move(sprite));
	addComponent(std::move(sr));
}

void ImageView::updateLayout() {
	View::updateLayout();
}

void ImageView::onMeasure(MeasureSpec horintalMeas, MeasureSpec verticalMeas) {

}

glm::vec2 ImageView::pivot() const
{
	const omen::ecs::SpriteRenderer* sprite = getComponent_const<omen::ecs::SpriteRenderer>();
	return sprite->pivot();
}

void ImageView::setPivot(const glm::vec2& pivot)
{
	omen::ecs::SpriteRenderer* sprite = getComponent<omen::ecs::SpriteRenderer>();
	sprite->setPivot(pivot);
}