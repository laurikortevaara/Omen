#pragma once

#include <string>

namespace omen {
	class Object {
	public:
		Object(const std::string &name);

		uint64_t id() const { return m_id; }
		const std::string& name() const { return m_name;  }
		void setName(const std::string& name) { m_name = name; }

	protected:
	private:
		uint64_t m_id;
		std::string m_name;
		
		static uint64_t id_counter;
	};
}