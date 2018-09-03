//
// Created by Lauri Kortevaara on 20/12/15.
//

#ifndef OMEN_SIGNAL_H
#define OMEN_SIGNAL_H

#include "Object.h"

#include <vector>
#include <map>
#include <tuple>

namespace omen {
    template <typename T, typename... Args>
    class Signal {
    public:
		Signal()
		{
			int i = 1;
			/*
						if (dynamic_cast<omen::ecs::Entity*>(owner)) {
				omen::ecs::Entity* e = dynamic_cast<omen::ecs::Entity*>(owner);
				e->signal_entity_destructed.connect(e,[&](omen::ecs::Entity* e) {
					auto it = std::find_if(m_observers.begin(), m_observers.end(), [e](const std::pair<omen::Object*, T>& elem) { return elem.first == e; });
					m_observers.erase(it);
				});
			}
			*/
		}

		template<typename T>
        void connect(omen::Object* owner, T obs) 
		{
            m_observers.push_back(std::pair<omen::Object*,T>(owner,obs));
        }

		void connect(void* owner, T obs) 
		{
			m_observers.push_back(std::pair<omen::Object*, T>(reinterpret_cast<omen::Object*>(owner), obs));
		}

		// Allow connecting a static method to this signal
		void connect_static(T obs) 
		{
			m_observers.push_back(std::pair<omen::Object*, T>(reinterpret_cast<omen::Object*>(nullptr), obs));
		}

		void removeObserver(omen::Object* o)
		{
			std::vector<std::pair<omen::Object*, T>>::iterator iter = m_observers.begin();
			for (auto obs : m_observers)
			{
				if (obs.first == o)
					break;
				iter++;
			}
			if (iter != m_observers.end())
				m_observers.erase(iter);
		}

        template<typename... a>
        void notify(a... args) {
			std::vector<std::pair<omen::Object*, T>> alive;
			for (auto obs : m_observers) 
			{
				if (obs.first == nullptr)
					alive.push_back(obs);
				auto it = std::find(Signals::observers_alive.begin(), Signals::observers_alive.end(), obs.first);
				if (it != Signals::observers_alive.end())
					alive.push_back(obs);
			}
			m_observers = alive;
			/*m_observers.clear();
			for (auto obs : alive)
				m_observers.push_back(obs);*/

            for(auto obs : m_observers)
                obs.second(std::forward<a>(args)...);
        }

    private:
        std::vector<std::pair<omen::Object*,T>> m_observers;
    };

	class Signals 
	{
	public:
		static void removeObserver(omen::Object* o);
		static std::vector<omen::Object*> observers_alive;
	};
}


#endif //OMEN_SIGNAL_H
