#include <iostream>
#include <fstream>
#include <sstream>
#include <clocale>
#include <string>
#include <vector>
#include <set>

std::vector<char> readIndex(std::string const& fileName)
{
	std::ifstream fin;
	fin.open(fileName, std::ios::in | std::ios::binary);
	if(!fin)
		return std::vector<char>();

	// Получаем размер файла.
	fin.seekg(0, std::ios::end);
	std::streampos const size = fin.tellg();
	fin.seekg(0, std::ios::beg);

	std::vector<char> buf/*(size)*/;
	buf.resize(size);
	fin.read(&buf[0], size);

	fin.close();
	return buf;
}

std::set<unsigned int> getDocID(std::vector<char> const& data, unsigned int const termID)
{
	if(data.empty())
		return std::set<unsigned int>();
	unsigned int const* iter = (unsigned int*)(&data[0] + 16);
	unsigned int const* const end = (unsigned int*)(&data[0] + data.size());
	while(true)
	{
		if(iter[0] != 0xAAAAAAAA || iter >= end)
			return std::set<unsigned int>();
		if(iter[1] == termID)
		{
			int a = 0;
		}
		iter += 4 + iter[2];
	}
}

int main(int argc, char** argv)
{
	std::setlocale(LC_CTYPE, "Russian");

	if(argc != 3)
	{
		std::cout << "IR4.exe index.dat запрос" << std::endl;
		return 1;
	}

	// Считываем весь индекс целиком в ОЗУ.
	std::vector<char> const rawIndex = readIndex(argv[1]);
	getDocID(rawIndex, 0);

	// Получим поисковой запрос.
	std::stringstream ss(argv[2]);
	return 0;
}
