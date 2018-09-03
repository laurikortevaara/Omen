//
// Created by Lauri Kortevaara on 06/02/16.
//
#include "ui/Layout.h"

#include "component/Component.h"


using namespace omen;
using namespace ui;

Layout::Layout(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size):
        View(parentView,name, pos, size) {

}
