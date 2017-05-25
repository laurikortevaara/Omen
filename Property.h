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

		typedef Signal<std::function<void(omen::Property*)> > PropertyChanged;
		static PropertyChanged signal_static_property_changed;

		double doubletValue() const {
			try { return std::any_cast<double>(*this); }
			catch (std::bad_any_cast) { return static_cast<double>(floatValue()); }
		}

		float floatValue() const {
			try { return std::any_cast<float>(*this); }
			catch (std::bad_any_cast) { return static_cast<float>(longValue()); }
		}

		int intValue() const {
			try { return std::any_cast<int>(*this); }
			catch (std::bad_any_cast) { return 0; }
		}

		long longValue() const {
			try { return std::any_cast<long>(*this); }
			catch (std::bad_any_cast) { return static_cast<long>(intValue()); }
		}

		std::string stringValue() const {
			try { return std::any_cast<std::string>(*this); }
			catch (std::bad_any_cast) { return static_cast<std::string>(charArrayValue()); }
		}

		const char* charArrayValue() const {
			try { return std::any_cast<const char*>(*this); }
			catch (std::bad_any_cast) { return ""; }
		}


		Property& operator=(const std::any&& other) {
			std::any::operator=(other);
			std::any& s = *this;
			signal_property_changed.notify(this);
			signal_static_property_changed.notify(this);
			return *this;
		}

		/*Property& operator=(const int& i) {
			*this = i;
			return *this;
		}*/
	};
}
