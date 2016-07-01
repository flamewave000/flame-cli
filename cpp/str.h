#pragma once
#ifndef _STR_
#define _STR_

#ifndef _STRING_
#include <string>
#endif
#ifndef _VECTOR_
#include <vector>
#endif


namespace str
{
	extern const std::string whitespace;

	inline bool contains(std::string str, const char &c)
	{
		return str.find_first_of(c) != std::string::npos;
	}
	inline bool contains(std::string target, std::string query)
	{
		return target.find(query) != std::string::npos;
	}

	std::vector<std::string> split(std::string str, const char &delim);

	std::string join(std::vector<std::string> tokens, const char &delim);
	std::string join(std::vector<std::string> tokens, std::string delim);

	inline std::string lpad(std::string str, int width, const char pad = ' ')
	{
		width -= str.size();
		return width > 0 ? str.insert(0, width, pad) : str;
	}
	inline std::string rpad(std::string str, int width, const char pad = ' ')
	{
		int size = str.size();
		width -= size;
		return width > 0 ? str.insert(size, width, pad) : str;
	}

	inline std::string ltrim(std::string str)
	{
		str.erase(str.begin(), str.begin() + str.find_first_not_of(whitespace));
		return str;
	}
	inline std::string rtrim(std::string str)
	{
		str.erase(str.begin() + str.find_last_not_of(whitespace) + 1, str.end());
		return str;
	}
	inline std::string trim(std::string str)
	{
		return rtrim(ltrim(str));
	}
}
#endif