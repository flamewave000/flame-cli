#include "stringx.hpp"
#include "cli.h"
#include <queue>
#include <iostream>
#include <sstream>

#define LINQ_USE_MACROS
#include <linq.hpp>
#include <stringx.hpp>

using namespace std;
using namespace strx;
using namespace linq;


#pragma region _flag Definition
#pragma region Constructors
Flag::Flag(void(*call)(string), string shortName, string longName, string description, Data data, bool required)
	: shortName(shortName), longName(longName), description(move(description)), data(data), call(call), required(required) { }
#pragma endregion


#pragma region Public Methods
string Flag::toString()
{
	return shortName.size() > 0 ? shortName : longName;
}
#pragma endregion
#pragma endregion


#pragma region CLI Definition
#pragma region Constructors
CLI::CLI(function<int(vector<string>)> start, vector<std::shared_ptr<Flag>> flags, const int &argc, const char* argv[], string description)
	: startDelegate(start),
	flags(std::move(flags)),
	description(description),
	argv(argc)
{
	for (int c = 0; c < argc; c++)
	{
		this->argv[c] = string(argv[c]);
	}
}
#pragma endregion


#pragma region Public Methods
int CLI::run()
{
	queue<string> args;
	linq_vec<Flag*> reqs = (flags WHERE { return item->required; } SELECT(Flag*) { return item.get(); });
	vector<string> unknown;
	if (argv.size() > 1 && argv[1] == "--help")
	{
		showHelp();
		return EXIT_SUCCESS;
	}
	for (size_t c = 1, size = argv.size(); c < size; c++)
	{
		args.push(argv[c]);
	}
	Flag *flag;
	while (!args.empty())
	{
		string rawArg = args.front();
		args.pop();
		pair<bool, string> arg = getArg(rawArg);
		flag = getFlag(arg.second, arg.first);
		if (flag == nullptr)
		{
			unknown.push_back(rawArg);
			continue;
		}
		reqs = reqs.where([&](auto item) { return flag != item; });
		string data = "";
		if (flag->data != Data::NoData)
		{
			if (arg.second == flag->shortName)
			{
				pair<bool, string> tmp = !args.empty() ? getArg(args.front()) : pair<bool, string>();
				bool isFlag = getFlag(tmp.second, tmp.first) != nullptr;
				if (flag->data == Data::Required && (args.empty() || isFlag))
				{
					showHelp("Expected data for flag '" + arg.second + "'");
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
					showHelp("Expected data for flag '" + arg.second + "'");
					return EXIT_FAILURE;
				}
			}
		}
		flag->call(data);
	}
	if (reqs.size() > 0)
	{
		vector<string> strs;
		for (auto req : reqs)
		{
			strs.push_back(req->toString());
		}
		showHelp("Missing required arguments [" + join(strs, ", ") + ']');
		return EXIT_FAILURE;
	}
	return startDelegate(unknown);
}
void CLI::showHelp(string error)
{
	if (error.size() != 0)
	{
		cerr << error << '\n';
	}
	if (description.size() == 0)
		printf("usage: %s [options]\n", argv[0].c_str());
	else
		cout << description << '\n';
	size_t shortLen = 0, longLen = 0, snameLen = 0, lnameLen = 0;
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
	string shortName;
	string longName;
	string desc;
	string cmdStr;
	vector<size_t> newlineIndecies;
	size_t count, max;
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
		for (size_t c = 0, size = desc.size(); c < size; c++)
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
		vector<string> lines = split(desc, '\n');
		desc = join(lines, rpad("\n", cmdStr.size() + 1));
		cerr << cmdStr << desc << endl;
	}
}
#pragma endregion


#pragma region Helper Methods
pair<bool, string> CLI::getArg(string arg)
{
	if (arg.size() == 0)
	{
		return pair<bool, string>(false, string(nullptr));
	}
	return strx::contains(arg, '=')
		? pair<bool, string>(true, strx::split(arg, '=')[0])
		: pair<bool, string>(false, arg);
}
Flag* CLI::getFlag(string name, bool isLong)
{
	return name.size() != 0
		? (flags FIRST_OR_DEFAULT(nullptr) { return isLong ? item->longName == name : item->shortName == name; }).get()
		: nullptr;
}
#pragma endregion  
#pragma endregion
