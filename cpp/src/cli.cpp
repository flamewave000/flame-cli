#include "cli.h"
#include <queue>
#include <iostream>
#include <sstream>

#define LINQ_USE_MACROS
#include <linq.hpp>

using namespace std;
using namespace strx;
using namespace linq;


#pragma region _flag Definition
#pragma region Constructors
Flag::Flag(uint32_t id, string shortName, string longName, string description, Data data, bool required)
	: id(id), shortName(shortName), longName(longName), description(move(description)), data(data), required(required) { }
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
CLI::CLI(std::vector<std::shared_ptr<Flag>> &&flags, std::string &&description)
	: flags(flags), description(description) {}
#pragma endregion


#pragma region Public Methods
int CLI::run(start_delegate &&start, const int &argc, const char* argv[])
{
	queue<string> args;
	linq_vec<Flag*> reqs = (flags WHERE{ return item->required; } SELECT(Flag*) { return item.get(); });
	vector<string> unknown;
	std::unordered_map<uint32_t, flag_pair> set_flags;
	if (argc > 1 && argv[1] == "--help")
	{
		showHelp();
		return EXIT_SUCCESS;
	}
	for (int c = 1; c < argc; c++)
	{
		args.push(argv[c]);
	}
	Flag *flag;
	while (!args.empty())
	{
		string rawArg = args.front();
		args.pop();
		string arg = getArg(rawArg);
		flag = getFlag(arg);
		if (flag == nullptr)
		{
			unknown.push_back(rawArg);
			continue;
		}
		reqs = reqs.where([&](auto item) { return flag != item; });
		string data = "";
		if (flag->data != Data::NoData)
		{
			if (arg == flag->shortName)
			{
				string tmp = !args.empty() ? getArg(args.front()) : "";
				bool isFlag = getFlag(tmp) != nullptr;
				if (flag->data == Data::Required && (args.empty() || isFlag))
				{
					showHelp("Expected data for flag '" + arg + "'");
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
					auto tokens = split(rawArg, '=');
					if (tokens.size() > 1) tokens.erase(tokens.begin());
					data = tokens.size() >= 1 ? join(tokens, '=') : string("");
				}
				else
				{
					showHelp("Expected data for flag '" + arg + "'");
					return EXIT_FAILURE;
				}
			}
		}
		set_flags[flag->id] = flag_pair{ *flag, data };
		//flag->call(data);
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
	return start(unknown, set_flags);
}

constexpr size_t min(const size_t &a, const size_t &b) {
	return a > b ? b : a;
}
void CLI::showHelp(string_view error, string_view pname)
{
	const size_t MAX_LENGTH = 80;

	format fmt = error.empty() ? ""_f : "{#}\n"_f % error;
	if (description.empty()) {
		fmt += "usage: {#} [options]\n";
		fmt.append(pname);
	}
	else {
		fmt += "{#}\n";
		fmt.append(description);
	}
	cerr << fmt.strclr();

	size_t shortLen = flags.max<size_t>([](auto item) { return item->shortName.size(); });
	size_t longLen = flags.max<size_t>([](auto item) { return item->longName.size(); });

	string shortName;
	string longName;
	string desc;
	string cmdStr;
	size_t count, max, c, size;
	for (auto flag : flags)
	{
		shortName = trim(flag->shortName);
		longName = trim(flag->longName);
		if (longName.empty()) longName = "    ";
		desc = trim(flag->description);
		cmdStr = rpad(shortName, shortLen + 2) + rpad(longName, longLen + 2);
		count = 0;
		max = min(MAX_LENGTH, MAX_LENGTH - cmdStr.size());
		for (c = 0, size = desc.size(); c < size; c++)
		{
			count = desc[c] == '\n' ? 0 : count + 1;
			if (count == max)
			{
				desc.insert(c, 1, '\n');
				c++;
				count = 0;
			}
		}
		auto lines = split(desc, '\n');
		desc = join(lines, rpad("\n", cmdStr.size() + 1)) + '\n';
		cerr << cmdStr << desc;
	}
}
#pragma endregion


#pragma region Helper Methods
string CLI::getArg(string arg)
{
	if (arg.size() == 0)
	{
		return string(nullptr);
	}
	return strx::contains(arg, '=')
		? strx::split(arg, '=')[0]
		: arg;
}
Flag* CLI::getFlag(string name)
{
	return name.size() != 0
		? (flags FIRST_OR_DEFAULT(nullptr) { return item->longName == name || item->shortName == name; }).get()
		: nullptr;
}
#pragma endregion  
#pragma endregion
