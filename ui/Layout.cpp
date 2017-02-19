//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#include "../component/Component.h"
#include "Layout.h"

using namespace omen;
using namespace ui;

Layout::Layout(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size):
        View(parentView,name, pos, size) {

}
