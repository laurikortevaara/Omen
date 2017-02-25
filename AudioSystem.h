//
// Created by Lauri Kortevaara on 12/11/16.
//

#ifndef OMEN_AUDIOSYSTEM_H
#define OMEN_AUDIOSYSTEM_H


#include "system/System.h"
#include <thread>

struct IGraphBuilder;
struct IMediaControl;
struct IMediaEvent;
struct IBasicAudio;

namespace omen {
	namespace ecs {
		class AudioSystem : public ecs::System {

		public:
			AudioSystem();

			virtual void add(Component *component);
			virtual void shutDown() {};

			void playAudio();

			long getVolume();
			long setVolume(long volume);

		protected:
		private:
			std::thread* m_audioThread;

			IGraphBuilder *pGraph;
			IMediaControl *pControl;
			IMediaEvent   *pEvent;
			IBasicAudio   *basicAudio;

		};
	}
}

#endif //OMEN_AUDIOSYSTEM_H
