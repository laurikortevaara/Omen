//
// Created by Lauri Kortevaara(personal) on 28/11/15.
//

#ifndef OMEN_SIGNAL_H
#define OMEN_SIGNAL_H

#include <map>
#include <ostream>
#include <iostream>
#include <vector>

// note that the Func parameter is something
// like std::function< void(int,int) > or whatever, greatly simplified
// by the C++11 standard

template<typename Function>
class Signal {
public:
    Signal() { }

    ~Signal() {
        std::cout << "Deleting signal" << __PRETTY_FUNCTION__ << std::endl;
        std::vector<void*> keys;
        for (auto f : m_functions) keys.push_back(f.first);
        for (auto f : keys)
            disconnect(f);
    }

    std::map<void *, Function> m_functions;

    void connect(void *observer, Function f) {
        std::cout << "Connecting: " << observer << std::endl;
        m_functions[observer] = f;
    }

    void connect(Function f) { connect(&f, f); }

    void disconnect(void *observer) {
        std::cout << "Distonnecting: " << observer << std::endl;
        m_functions.erase(observer);
    }

    template<typename T, typename... Args>
    void emit(T first, Args... args) { for (auto f : m_functions) f.second(first, args...); };
};

#endif //OMEN_SIGNAL_H
