#include <iostream>

#include "wx/app.h"

#include "tech_demo.hpp"

int main(int argc, char ** argv)
{
	try {
		TechDemo::run_all();
		return 0;
	} catch (std::exception e) {

		std::cout << e.what() << '\n';
		return 1;
	}
}
