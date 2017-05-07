#include "Object.h"
#include "Signal.h"
#include <any>

using namespace omen;

uint64_t omen::Object::id_counter = 0;

Object::Object(const std::string& name) : m_id(++id_counter), m_name(name) 
{
	omen::Signals::observers_alive.push_back(this);
}

Object::~Object()
{
	omen::Signals::removeObserver(this);
}