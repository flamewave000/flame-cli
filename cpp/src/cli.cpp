#include "stringx.hpp"
#include "cli.h"
#include <queue>
#include <iostream>
#include <sstream>

using namespace std;
using namespace strx;


#pragma region _flag Definition
#pragma region Constructors
_flag::_flag(void(*call)(std::string), std::string shortName, std::string longName, std::string description, Data data, bool required)
	: shortName(shortName), longName(longName), description(description), data(data), call(call), required(required) { }
#pragma endregion


#pragma region Public Methods
string _flag::toString()
{
	return shortName.size() > 0 ? shortName : longName;
}
#pragma endregion
#pragma endregion


#pragma region Flag Definition
#pragma region Constructors
Flag::Flag() : std::shared_ptr<_flag>() { }
Flag::Flag(_flag * _Px) : std::shared_ptr<_flag>(_Px) { }
Flag::Flag(void(*call)(std::string), std::string shortName, std::string longName, std::string description, Data data, bool required)
	: std::shared_ptr<_flag>(new _flag(call, shortName, longName, description, data, required)) { }
#pragma endregion
#pragma endregion


#pragma region CLI Definition
#pragma region Constructors
CLI::CLI(int(*start)(std::vector<std::string>), vector<Flag> flags, int argc, const char* argv[], std::string description)
{
	this->startDelegate = start;
	this->flags = flags;
	this->description = description;
	this->argv = vector<std::string>(argc);
	for (int c = 0; c < argc; c++)
	{
		this->argv[c] = std::string(argv[c]);
	}
}
#pragma endregion


#pragma region Public Methods
int CLI::run()
{
	queue<std::string> args;
	queue<Flag> flags;
	vector<Flag> reqs;
	for (auto req : this->flags)
	{
		if (req->required)
		{
			reqs.push_back(req);
		}
	}
	vector<std::string> unknown;
	if (argv.size() > 1 && argv[1] == "--help")
	{
		showHelp();
		return EXIT_SUCCESS;
	}
	for (int c = 1, size = argv.size(); c < size; c++)
	{
		args.push(argv[c]);
	}
	Flag flag;
	while (!args.empty())
	{
		std::string rawArg = args.front();
		args.pop();
		tuple<bool, std::string> arg = getArg(rawArg);
		flag = getFlag(get<1>(arg), get<0>(arg));
		if (flag == nullptr)
		{
			unknown.push_back(rawArg);
			continue;
		}
		for (int c = 0, size = reqs.size(); c < size; c++)
		{
			if (flag == reqs[c])
			{
				reqs.erase(reqs.begin() + c);
				break;
			}
		}
		std::string data = "";
		if (flag->data != Data::NoData)
		{
			if (get<1>(arg) == flag->shortName)
			{
				tuple<bool, std::string> tmp = !args.empty() ? getArg(args.front()) : tuple<bool, std::string>();
				bool isFlag = getFlag(get<1>(tmp), get<0>(tmp)) != nullptr;
				if (flag->data == Data::Required && (args.empty() || isFlag))
				{
					showHelp("Expected data for flag '" + std::get<1>(arg) + "'");
					return EXIT_FAILURE;
				}
				else if (flag->data == Data::Required || (flag->data == Data::Optional && !args.empty() && !isFlag))
				{
					data = args.front();
					args.pop();
				}
			}
			else
			{
				if (contains(rawArg, '='))
				{
					data = join(split(rawArg, '='), '=');
				}
				else
				{
					showHelp("Expected data for flag '" + std::get<1>(arg) + "'");
					return EXIT_FAILURE;
				}
			}
		}
		flag->call(data);
	}
	if (reqs.size() > 0)
	{
		std::vector<std::string> strs;
		for (auto req : reqs)
		{
			strs.push_back(req->toString());
		}
		showHelp("Missing required arguments [" + join(strs, ", ") + ']');
		return EXIT_FAILURE;
	}
	return startDelegate(unknown);
}
void CLI::showHelp(std::string error)
{
	if (error.size() != 0)
	{
		cerr << error << '\n';
	}
	if (description.size() == 0)
		printf("usage: %s [options]\n", argv[0].c_str());
	else
		cout << description << '\n';
	int shortLen = 0, longLen = 0, snameLen = 0, lnameLen = 0;
	for (auto flag : flags)
	{
		if (flag->shortName.size() != 0)
		{
			snameLen = flag->shortName.size();
			shortLen = snameLen > shortLen ? snameLen : shortLen;
		}
		if (flag->longName.size() != 0)
		{
			lnameLen = flag->longName.size();
			longLen = lnameLen > longLen ? lnameLen : longLen;
		}
	}
	std::string shortName;
	std::string longName;
	std::string desc;
	std::string cmdStr;
	std::vector<int> newlineIndecies;
	int count, max;
	for (auto flag : flags)
	{
		shortName = flag->shortName.size() != 0 && trim(flag->shortName).size() ? flag->shortName : "";
		longName = flag->longName.size() != 0 && trim(flag->longName).size() ? flag->longName : "    ";
		desc = flag->description.size() != 0 ? flag->description : "";
		ostringstream sstream;
		sstream
			<< rpad(shortName, shortLen + 2)
			<< rpad(longName, longLen + 2);
		cmdStr = sstream.str();

		newlineIndecies.clear();
		count = 0;
		max = 80 - cmdStr.size();
		for (int c = 0, size = desc.size(); c < size; c++)
		{
			if (desc[c] == '\n')
				count = 0;
			else
				count += 1;
			if (count == max)
			{
				newlineIndecies.push_back(c);
				count = 0;
			}
		}
		count = 0;
		for (auto index : newlineIndecies)
		{
			desc.insert(desc.begin() + index, '\n');
			count += 1;
		}
		std::vector<std::string> lines = split(desc, '\n');
		desc = join(lines, rpad("\n", cmdStr.size() + 1));
		cerr << cmdStr << desc << endl;
	}
}
#pragma endregion


#pragma region Helper Methods
tuple<bool, std::string> CLI::getArg(std::string arg)
{
	if (arg.size() == 0)
	{
		return std::make_tuple(false, std::string(nullptr));
	}
	return strx::contains(arg, '=') ?
		std::make_tuple(true, strx::split(arg, '=')[0])
		: std::make_tuple(false, arg);
}
Flag CLI::getFlag(std::string name, bool isLong)
{
	if (name.size() == 0)
	{
		return nullptr;
	}
	for (auto flag : flags)
	{
		if (isLong)
		{
			if (flag->longName == name)
			{
				return flag;
			}
		}
		else if (flag->shortName == name)
		{
			return flag;
		}
	}
	return nullptr;
}
#pragma endregion  
#pragma endregion
