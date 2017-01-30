//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#include "Sky.h"
#include "GL_error.h"
#include "Engine.h"

using namespace omen;

#include "Sky.h"
#include "component/MeshController.h"
#include "component/MeshRenderer.h"
#include "SkyRenderer.h"
#include "MeshProvider.h"

using namespace omen;

Sky::Sky() :
	GameObject("Sky")
{
	std::unique_ptr<ecs::MeshController> mc = std::make_unique<ecs::MeshController>();
	std::unique_ptr<ecs::SkyRenderer> mr = std::make_unique<ecs::SkyRenderer>(mc.get());

	std::unique_ptr<Mesh> mesh = MeshProvider::createPlane(1000, 1);

	mc->setMesh(std::move(mesh));
	addComponent(std::move(mr));
	addComponent(std::move(mc));
}


Sky::~Sky()
{
}




