#include "EditorCameraView.h"
#include "../Engine.h"
#include "../component/MeshController.h"
#include "../component/MeshRenderer.h"
#include "../component/EditorSceneRenderer.h"
#include "../MeshProvider.h"
#include "../Color.h"

using namespace omen;
using namespace omen::editor;
using namespace omen::ui;

EditorCameraView::EditorCameraView() :
	EditorWindow("EditorCameraView")
{
	layoutParams().layoutSizingWidth = LayoutParams::MatchParent;
	layoutParams().layoutSizingHeight = LayoutParams::MatchParent;
	children().clear();
	setBackground(Color("#0000ff"));
	initialize();
}

EditorCameraView::~EditorCameraView()
{
}

void EditorCameraView::initialize()
{
	addComponent(std::make_unique<omen::ecs::EditorSceneRenderer>());

	Engine::instance()->window()->signal_file_dropped.connect(this, [this](const std::vector<std::string>& files)
	{
		const std::string& file = *files.begin();
		if (file.find(".MD3") != std::string::npos
			|| file.find(".md3") != std::string::npos) {
			Engine::instance()->scene()->loadModel(file);
		}
		else
			if (file.find(".FBX") != std::string::npos
				|| file.find(".fbx") != std::string::npos) {
				std::unique_ptr<ecs::GameObject> obj = Engine::instance()->scene()->createObject(file);

				if (true)
				{
					std::unique_ptr<omen::ecs::GameObject> obj = std::make_unique<omen::ecs::GameObject>("obj");

					std::unique_ptr<omen::ecs::MeshController> mc = std::make_unique<omen::ecs::MeshController>();
					mc->setMesh(std::move(MeshProvider::createPlane(10000)));
					std::unique_ptr<omen::ecs::MeshRenderer> mr = std::make_unique<omen::ecs::MeshRenderer>(mc.get());
					obj->addCompnent(std::move(mr));
					obj->addCompnent(std::move(mc));

					obj->tr()->pos().x = -5000;
					obj->tr()->pos().y = -2;
					obj->tr()->pos().z = -5000;
					obj->setName("plane");
					/*Engine::instance()->signal_engine_update.connect(this,[file](float time, float delta_time) {
					ecs::GameObject* obj = dynamic_cast<ecs::GameObject*>(Engine::instance()->scene()->findEntity(file));
					obj->tr()->rotate(time, glm::vec3(0.75, 0, 0));
					obj->tr()->rotate(time, glm::vec3(0, 0.34, 0));
					});*/
					Engine::instance()->scene()->addEntity(std::move(obj));
				}
			}/*
			 else if (file.find(".JPG") != std::string::npos
			 || file.find(".jpg") != std::string::npos) {
			 Texture* t = new Texture(file);
			 }*/
	});
}