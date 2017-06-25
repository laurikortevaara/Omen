#include "WindowDivider.h"
#include "../ui/LinearLayout.h"
#include "../component/MouseInput.h"
#include "../Engine.h"

WindowDivider::WindowDivider(GUILayout* parent) :
	View(nullptr, "WinDiv", { 0.0f,0.0f }, { 0.0f,0.0f })
{
	omen::ui::LinearLayout* layout = reinterpret_cast<omen::ui::LinearLayout*>(parent->children().front().get());
	if (layout->layoutDirection() == omen::ui::LinearLayout::LayoutDirection::HORIZONTAL)
		setSize2D({ 5.0f, parent->height() });
	else
		setSize2D({ parent->width(), 5.0f });

	tr()->setBounds(glm::vec3(0, 0, 0), glm::vec3(width(), height(), 0));

	signal_entered.connect(this, [this](omen::ecs::Entity* e, glm::vec2 pos) 
	{
		glfwSetCursor(omen::Engine::instance()->window()->window(), glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
	});

	signal_exited.connect(this, [this](omen::ecs::Entity* e, glm::vec2 pos)
	{
		glfwSetCursor(omen::Engine::instance()->window()->window(), glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
	});
}