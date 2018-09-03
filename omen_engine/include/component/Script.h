#ifndef OMEN_SCRIPT_COMPONENT
#define OMEN_SCRIPT_COMPONENT

#include "Component.h"

namespace omen {
	namespace ecs {
		class Script : public Component {
		protected:
			virtual void onAttach(ecs::Entity* e) {};
			virtual void onDetach(ecs::Entity* e) {};
		public:
			Script(const char* scriptFile);
			void update(double time, double deltaTime);

			
		};
	}
}

#endif
