//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#include "ImageView.h"
#include "../component/Sprite.h"
#include "../component/SpriteRenderer.h"
#include "../Engine.h"
#include "../../component/Sprite.h"
#include "../../component/BorderSprite.h"

using namespace omen;
using namespace ui;

ImageView::~ImageView() = default;

ImageView::ImageView(View* parentView, const std::string &name,const std::string &spriteName, const glm::vec2& pos,  int width,  int height) :
        View(parentView, name)
{
	setPos(pos);
	setSize(glm::vec2(width, height));
	std::unique_ptr<Transform> tr = std::make_unique<Transform>();
	tr->pos().x = pos.x;
	tr->pos().y = pos.y;
	tr->pos().z = -1;
	tr->setBounds(glm::vec3(0, 0, 0), glm::vec3(width, height, 0));

	std::unique_ptr<omen::ecs::Sprite> sprite = nullptr;
	sprite = std::make_unique<omen::ecs::Sprite>(spriteName, pos, width, height);
	if (width == -1 && height == -1)
		tr->setBounds(glm::vec3(0, 0, 0), glm::vec3(sprite->texture()->width(), sprite->texture()->height(), 0));
	std::unique_ptr<omen::ecs::SpriteRenderer> sr = std::make_unique<omen::ecs::SpriteRenderer>(std::move(sprite));

	addComponent(std::move(sr));
	addComponent(std::move(tr));
}

void ImageView::updateLayout() {

}

void ImageView::onMeasure(float maxwidth, float maxheight) {

}
