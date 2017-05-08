#include <functional>
#include "Signal.h"
#include "Property.h"

omen::Property::PropertyChanged omen::Property::signal_static_property_changed;

float omen::Property::floatValue() const
{
	float f = *std::any_cast<float>(reinterpret_cast<std::any*>(const_cast<Property*>(this)));
	return f; 
}