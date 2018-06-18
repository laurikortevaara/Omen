//
// Created by Lauri Kortevaara on 11/01/16.
//

#include "SkyBox.h"
#include "GL_error.h"
#include "Engine.h"

using namespace omen;

#include "SkyBox.h"
#include "component/MeshController.h"
#include "component/MeshRenderer.h"
#include "SkyBoxRenderer.h"
#include "MeshProvider.h"

using namespace omen;

SkyBox::SkyBox() :
	GameObject("SkyBox")
{
	std::unique_ptr<ecs::MeshController> mc = std::make_unique<ecs::MeshController>();
	std::unique_ptr<ecs::SkyBoxRenderer> mr = std::make_unique<ecs::SkyBoxRenderer>(mc.get());

	std::unique_ptr<Mesh> mesh = MeshProvider::createCube();

	mc->setMesh(std::move(mesh));
	addComponent(std::move(mr));
	addComponent(std::move(mc));
}


SkyBox::~SkyBox()
{
}




