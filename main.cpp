#include <iostream>

int main(int argc, char** argv)
{
	setlocale(LC_CTYPE, "Russian");

	if(argc != 3)
	{
		std::cout << "IR3.exe tokens.xml index.dat" << std::endl;
		return 1;
	}

	return 0;
}
