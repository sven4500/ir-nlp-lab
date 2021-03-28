#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <clocale>
#include <string>
#include <vector>
#include <set>
#include <ctime>
#include <unicode/unistr.h>
#include <crc32.h>
#include "parser.h"

int main(int argc, char** argv)
{
	std::setlocale(LC_CTYPE, "Russian");

	if(argc != 2)
	{
		std::cout << "IR4.exe индекс" << std::endl;
		return 1;
	}

	std::ifstream fin;
	fin.open(argv[1], std::ios::binary);

	if(!fin)
	{
		std::cout << "Не удалось открыть индекс." << std::endl;
		return -1;
	}

	{
		std::string query;
		std::getline(std::cin, query);

		std::clock_t const clockBegin = clock();
		std::vector<unsigned int> const docID = parse(fin, query.c_str());
		std::clock_t const clockEnd = clock();

		for(std::size_t i = 0; i < docID.size(); ++i)
			std::cout << docID[i] << ' ';
		std::cout << std::endl;

		unsigned int const msTimeElapsed = (unsigned int)(((double)(clockEnd - clockBegin) / CLOCKS_PER_SEC) * 1000.0);
		std::cout << msTimeElapsed << std::endl;
	}
	return 0;
}
