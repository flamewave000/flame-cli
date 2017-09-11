#pragma once
#ifndef _CLI_
#define _CLI_

#ifndef _STRING_
#include <string>
#endif
#ifndef _VECTOR_
#include <vector>
#endif
#ifndef _MEMORY_
#include <memory>
#endif
#define LINQ_USE_MACROS
#include <linq.hpp>
#include <functional>


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

class CLI;
class Flag : public Base
{
public:
	std::string shortName; // Short name of flag(i.e. - i, -u john.doe).
	std::string longName; // Long name of flag (i.e. --interactive, --username=john.doe).
	bool required; // True if the argument is required.
	std::string description; // Description of what the argument does.
	Data data; // Takes the next argument in the argument list as data.
	void(*call)(std::string); // Method to be called, must take one parameter. Will be the paired data, or None if data is False.
private:
	Flag() {}
public:
	/*Initializes the Flag class*/
	Flag(void(*call)(std::string), std::string shortName, std::string longName, std::string description, Data data = Data::NoData, bool required = false);
public:
	virtual std::string toString();
	friend CLI;
};

class CLI : public Base
{
#pragma region Instance Variables
private:
	linq::linq_vec<std::shared_ptr<Flag>> flags;
	std::string description;
	std::function<int(std::vector<std::string>)> startDelegate;
	std::vector<std::string> argv;
#pragma endregion


#pragma region Constructors
public:
	CLI(std::function<int(std::vector<std::string>)> start, std::vector<std::shared_ptr<Flag>> flags, const int &argc, const char* argv[], std::string description = "");
#pragma endregion


#pragma region Public Methods
public:
	int run();
	void showHelp(std::string error = "");
#pragma endregion


#pragma region Helper Methods
private:
	std::pair<bool, std::string> getArg(std::string arg);
	Flag* getFlag(std::string name, bool isLong);
#pragma endregion
};

#endif
