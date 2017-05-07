#pragma once
#include <any>
#include <string>

namespace omen
{
	class Property : public std::any {
	public:
		std::string m_name;
		const std::string& name() const { return m_name; }
		Property(const char* name) : m_name(name) {}

		Property() : m_name("") {}

		typedef Signal<std::function<void(std::any*)> > Change;
		Change signal_property_changed;

		Property& operator=(const std::any&& other) {
			std::any::operator=(other);
			std::any& s = *this;
			signal_property_changed.notify(this);
			return *this;
		}
	};
}
