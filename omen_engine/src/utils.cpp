//
// Created by Lauri Kortevaara on 29/12/15.
//
#include <string>
#include "utils.h"
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef _WIN32
#include <direct.h>
    #define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd

#endif

namespace omen {
    std::string getWorkingDir() {
        char cCurrentPath[FILENAME_MAX];

        _getcwd(cCurrentPath, FILENAME_MAX);
        if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
            return "";
        }

        cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
        return std::string(cCurrentPath);
    }

	/*
	* split_string with given delimiter
	*/
	std::vector<std::string> split_string(const std::string& str,
		const std::string& delimiter)
	{
		std::vector<std::string> strings;
		std::string::size_type pos = 0;
		std::string::size_type prev = 0;
		while ((pos = str.find(delimiter, prev)) != std::string::npos)
		{
		strings.push_back(str.substr(prev, pos - prev));
		prev = pos + 1;
		}

		// To get the last substring (or only, if delimiter is not found)
		strings.push_back(str.substr(prev));
		return strings;
	}

	template <typename T>
	std::wstring to_string_with_precision(const T a_value, const int n)
	{
		std::owstringstream out;
		out << std::setprecision(n) << a_value;
		return out.wstr();
	}

	std::wstring string_to_wstring(const std::string& a_value)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(a_value);
		return wide;
	}
}
