#include "Renderer.h"

using namespace omen;
using namespace ecs;

void Renderer::onAttach(Entity *e) {
	m_entity = e;
}

void Renderer::onDetach(ecs::Entity *e) {

}
