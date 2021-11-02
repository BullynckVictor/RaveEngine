#include <iostream>
#include "Engine/Core/Main.h"

int main()
{
	int res = rave_main();
	std::cout << "Hello World from Engine!\n";
	std::cin.ignore();
	return res;
}