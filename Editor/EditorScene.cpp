//
// Created by Lauri Kortevaara on 08/11/15.
//

#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

#include "EditorScene.h"
#include "../Model.h"
#include "../component/MeshController.h"
#include "../utils.h"
#include "../GL_error.h"
#include "../Engine.h"
#include "../system/GraphicsSystem.h"
#include "../GameObject.h"
#include "../component/Picker.h"
#include "../ui/Button.h"
#include "../ui/Slider.h"
#include "../ui/LinearLayout.h"
#include "../ToolView.h"
#include "../ui/TextView.h"
#include "../Texture.h"
#include "../component/MouseInput.h"
#include "../MultipassShader.h"
#include "../MathUtils.h"
#include "../component/KeyboardInput.h"


using namespace omen;

namespace fs = std::experimental::filesystem;

EditorScene::EditorScene() : omen::Object("EditorScene") 
{
}


EditorScene::~EditorScene() {
}


void omen::EditorScene::initialize()
{
}

void EditorScene::render(const glm::mat4 &viewProjection, const glm::mat4 &view) 
{
	ecs::GraphicsSystem* gs = omen::Engine::instance()->findSystem<ecs::GraphicsSystem>();

	glClear(GL_DEPTH_BUFFER_BIT);
	gs->render(nullptr, 0);
	gs->render(nullptr, 1);
}


void omen::EditorScene::addEntity(std::unique_ptr<ecs::Entity> entity, GLuint layer)
{
	ecs::Entity* eptr = entity.get();
	if(entity->layer()==-1)
		entity->setLayer(layer);
	m_entities[layer].push_back(std::move(entity));
	signal_entity_added.notify(eptr);
}

ecs::Entity* EditorScene::findEntity(const std::string& name)
{
	for(const auto& key : m_entities)
	for (const auto& e : entities(key.first)) {
		if (e->name() == name)
			return e.get();
		ecs::Entity* c = e->findChild(name);
		if (c != nullptr)
			return c;
	}
	return nullptr;
}	