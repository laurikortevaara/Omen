//
// Created by Lauri Kortevaara(personal) on 20/12/15.
//

#ifndef OMEN_SIGNAL_H
#define OMEN_SIGNAL_H

#include <vector>
#include <map>

namespace Omen {

    template <typename T, typename... Args>
    class Signal {
    public:
        void connect(T obs) {
            m_observers.push_back(obs);
        }

        template<typename... a>
        void notify(a... args) {
            for(auto obs : m_observers)
                obs(std::forward<a>(args)...);
        }

    private:
        std::vector< T > m_observers;
    };
}


#endif //OMEN_SIGNAL_H
