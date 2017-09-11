#include <iostream>
#include "cli.h"

using namespace std;


int main(int argc, const char *argv[]) {
	CLI cli([](auto args) {
			cout << flush;
			cin.get();
			return 0;
		},
		vector<shared_ptr<Flag>>(
		{
			make_shared<Flag>([](auto data) {cout << "argument '-' provided" << flush; }, "-a", "--arg-a", "This is an example description for argument a. If the text is too long, it will be word wrapped when printed to the screen.", Data::NoData, true),
			make_shared<Flag>([](auto data) {cout << "argument '-' provided" << flush; }, "-b", "--arg-b", "This is another example description, but argument b.", Data::Required, false),
			make_shared<Flag>([](auto data) {cout << "argument '-' provided" << flush; }, "-c", "--arg-c", "And another description, but this time argument c.", Data::Optional, false),
			make_shared<Flag>([](auto data) {cout << "argument '-' provided" << flush; }, "-d", "", "And finally, argument d", Data::NoData, false)
		}),
		argc,
		argv,
		"Example CLI");

	return cli.run();
}
