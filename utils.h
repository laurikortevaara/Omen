//
// Created by Lauri Kortevaara(personal) on 29/12/15.
//

#ifndef OMEN_UTILS_H_H
#define OMEN_UTILS_H_H

#endif //OMEN_UTILS_H_H

#include <cstdlib>
#include <sstream>
#include <iomanip>

namespace omen {
    
	/*
	* inline random
	*/
	inline int random (int low, int high) {
        if (low > high) return high;
        return low + (std::rand() % (high - low + 1));
    }

	/*
	 * getWorkingDir
	*/
    std::string getWorkingDir();

	/*
	* to_string_with_precision
	*/
	template <typename T>
	std::string to_string_with_precision(const T a_value, const int n = 6)
	{
		std::ostringstream out;
		out << std::setprecision(n) << a_value;
		return out.str();
	}

	/*
	* to_wstring_with_precision
	*/
	template <typename T>
	std::wstring to_wstring_with_precision(const T a_value, const int n = 6)
	{
		std::wostringstream out;
		out << std::setprecision(n) << a_value;
		return out.str();
	}
}