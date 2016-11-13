//
// Created by Lauri Kortevaara(personal) on 12/11/16.
//

#ifndef OMEN_AUDIOSYSTEM_H
#define OMEN_AUDIOSYSTEM_H


#include "system/System.h"
#include <thread>

namespace omen {
	namespace ecs {
		class AudioSystem : public ecs::System {

		public:
			virtual void add(Component *component);
			void playAudio();

		protected:
		private:
			std::thread* m_audioThread;
		};
	}
}

#endif //OMEN_AUDIOSYSTEM_H
