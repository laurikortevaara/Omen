#include "StringTools.h"


std::string omen::StripComments(std::string str)
{
	if (str.find("//") != std::string::npos)
		return str.substr(0, str.find("//"));
	else
		return str;
}