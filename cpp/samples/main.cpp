#include <iostream>
#include "cli.h"

using namespace std;
using namespace strx;


int main(int argc, const char *argv[]) {
	CLI cli(
		vector<shared_ptr<Flag>>(
	{
		make_shared<Flag>(0x1, "-a", "--arg-a", "This is an example description for argument a. If the text is too long, it will be word wrapped when printed to the screen.", Data::NoData, false),
		make_shared<Flag>(0x2, "-b", "--arg-b", "This is another example description, but argument b.", Data::Required, false),
		make_shared<Flag>(0x4, "-c", "--arg-c", "And another description, but this time argument c.", Data::Optional, false),
		make_shared<Flag>(0x8, "-d", "", "And finally, argument d", Data::NoData, false)
	}), "Example CLI");

	return cli.run([&cli](auto unknown, unordered_map<uint32_t, CLI::flag_pair> flags) {
		cout << flush;

		for (auto flag : flags) {
			cout << (format(string() +
				"         id: {#}\n" +
				"  shortName: {#}\n" +
				"   longName: {#}\n" +
				"   required: {#}\n" +
				"description: {#}\n" +
				" wants data: {#}\n" +
				"       data: \"{#}\"\n\n")
				% flag.second.flag.id
				% flag.second.flag.shortName
				% flag.second.flag.longName
				% (flag.second.flag.required ? "true" : "false")
				% flag.second.flag.description
				% (flag.second.flag.data == Data::NoData ? "No" : (flag.second.flag.data == Data::Optional ? "Optional" : "Required"))
				% flag.second.data);
		}
		cli.showHelp();
		cout << "Press enter to exit..." << flush;
		cin.get();
		return 0;
	}, argc, argv);
}
