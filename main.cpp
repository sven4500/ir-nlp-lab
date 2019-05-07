#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <clocale>
#include <string>
#include <vector>
#include <set>
#include <unicode/unistr.h>
#include <crc32.h>
#include "parser.h"

int main(int argc, char** argv)
{
	std::setlocale(LC_CTYPE, "Russian");

	if(argc != 3)
	{
		std::cout << "IR4.exe индекс запрос" << std::endl;
		return 1;
	}

	std::ifstream fin;
	fin.open(argv[1], std::ios::binary);

	if(!fin)
	{
		std::cout << "Ќе удалось открыть индекс." << std::endl;
		return -1;
	}

	{
		std::vector<unsigned int> const docID = parse(fin, argv[2]);
		for(std::size_t i = 0; i < docID.size(); ++i)
			std::cout << docID[i] << ' ';
	}

	return 0;
}
