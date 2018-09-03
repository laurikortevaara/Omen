#include "Signal.h"

using namespace omen;

std::vector<omen::Object*> omen::Signals::observers_alive;

void Signals::removeObserver(omen::Object* o)
{
	auto it = std::find(observers_alive.begin(), observers_alive.end(), o);
	if (it != observers_alive.end())
		observers_alive.erase(it);
}