#pragma once
#ifndef _CLI_
#define _CLI_

#ifndef _STRING_
#include <string>
#endif
#ifndef _VECTOR_
#include <vector>
#endif
#ifndef _TUPLE_
#include <tuple>
#endif
#ifndef _MEMORY_
#include <memory>
#endif

typedef enum _data
{
	NoData = 0,
	Optional = 1,
	Required = 2
} Data;

class Base
{
public:
	virtual std::string toString() { return typeid(*this).name(); }
};

class _flag : public Base
{
public:
	std::string shortName; // Short name of flag(i.e. - i, -u john.doe).
	std::string longName; // Long name of flag (i.e. --interactive, --username=john.doe).
	bool required; // True if the argument is required.
	std::string description; // Description of what the argument does.
	Data data; // Takes the next argument in the argument list as data.
	void(*call)(std::string); // Method to be called, must take one parameter. Will be the paired data, or None if data is False.
public:
	/*Initializes the Flag class*/
	_flag(void(*call)(std::string), std::string shortName, std::string longName, std::string description, Data data = Data::NoData, bool required = false);
public:
	virtual std::string toString();
};

class Flag : public std::shared_ptr<_flag>
{
public:
	Flag();
	Flag(_flag * _Px);
	Flag(void(*call)(std::string), std::string shortName, std::string longName, std::string description, Data data = Data::NoData, bool required = false);
};

class CLI : public Base
{
#pragma region Instance Variables
private:
	std::vector<Flag> flags;
	std::string description;
	int(*startDelegate)(std::vector<std::string>);
	std::vector<std::string> argv;
#pragma endregion


#pragma region Constructors
public:
	CLI(int(*start)(std::vector<std::string>), std::vector<Flag> flags, int argc, const char* argv[], std::string description = "");
#pragma endregion


#pragma region Public Methods
public:
	int run();
	void showHelp(std::string error = "");
#pragma endregion


#pragma region Helper Methods
private:
	std::tuple<bool, std::string> getArg(std::string arg);
	Flag getFlag(std::string name, bool isLong);
#pragma endregion
};

#endif
