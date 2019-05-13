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
		std::cin >> query;
		std::vector<unsigned int> const docID = parse(fin, query.c_str());
		for(std::size_t i = 0; i < docID.size(); ++i)
			std::cout << docID[i] << ' ';
	}

	return 0;
}
