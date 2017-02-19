//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#include "../component/Component.h"
#include "FrameLayout.h"

using namespace omen;
using namespace ui;

FrameLayout::FrameLayout(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size) :
        Layout(parentView, name, pos, size)
{

}

void FrameLayout::updateLayout() {

}

