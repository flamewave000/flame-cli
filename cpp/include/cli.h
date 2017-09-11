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

#ifndef _STRINGX_
#include <stringx.hpp>
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

class CLI;
class Flag : public Base
{
public:
	uint32_t id;
	std::string shortName; // Short name of flag(i.e. - i, -u john.doe).
	std::string longName; // Long name of flag (i.e. --interactive, --username=john.doe).
	bool required; // True if the argument is required.
	std::string description; // Description of what the argument does.
	Data data; // Takes the next argument in the argument list as data.
private:
	Flag() {}
public:
	/*Initializes the Flag class*/
	Flag(uint32_t id, std::string shortName, std::string longName, std::string description, Data data = Data::NoData, bool required = false);
public:
	virtual std::string toString();
	friend CLI;
};

class CLI : public Base
{
#pragma region Internal Types
public:
	struct flag_pair { Flag flag; std::string data; };
	typedef std::function<int(const std::vector<std::string>&, const std::unordered_map<uint32_t, flag_pair>&)> start_delegate;
#pragma endregion


#pragma region Instance Variables
private:
	std::string description;
	linq::linq_vec<std::shared_ptr<Flag>> flags;
#pragma endregion


#pragma region Constructors
public:
	CLI(std::vector<std::shared_ptr<Flag>> &&flags, std::string &&description = "");
#pragma endregion


#pragma region Public Methods
public:
	int run(start_delegate &&start, const int &argc, const char* argv[]);
	void showHelp(std::string_view error = "", std::string_view pname = "");
#pragma endregion


#pragma region Helper Methods
private:
	std::string getArg(std::string arg);
	Flag* getFlag(std::string name);
#pragma endregion
};

#endif
