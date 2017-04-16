#pragma once
#include "System.h"

namespace omen {
	namespace ecs {
		class ScriptSystem : public ecs::System
		{
			virtual void shutDown() {};
		};
	}
}
