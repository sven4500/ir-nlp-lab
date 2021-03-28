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

#ifdef _DEBUG
#include <Windows.h>
#endif

int main(int argc, char** argv)
{
	std::setlocale(LC_CTYPE, "Russian");

	if(argc != 3)
	{
		std::cout << "IR5.exe _In_index.dat _In_posindex.dat" << std::endl;
		return 1;
	}

    #ifdef _DEBUG
    // ������ �� ����� ������� ��������� ���� ������� �������������
    // ������ ���������� ���� ������������ �� ����� ������ OK.
    // ��� ���� ��� ����� ����� ��������� �������� � ���������.
    // ��� ����������� ���� ������� �������� ������ ��� �������
    // ��������� ������ ������.
    MessageBox(NULL, NULL, NULL, MB_OK);
    #endif

	std::ifstream finIndex, finPosindex;
	finIndex.open(argv[1], std::ios::in | std::ios::binary);
    finPosindex.open(argv[2], std::ios::in | std::ios::binary);

	if(!finIndex || !finPosindex)
	{
		std::cout << "�� ���� ������� ���� �������" << std::endl;
		return -1;
	}

    // �� ������������ ������ ����� �������� ������ ���������� ������.
	std::string query;
	std::getline(std::cin, query);

	std::clock_t const clockBegin = clock();
	std::vector<unsigned int> const docID = parse(finIndex, finPosindex, query.c_str());
	std::clock_t const clockEnd = clock();

	for(std::size_t i = 0; i < docID.size(); ++i)
		std::cout << docID[i] << ' ';
	std::cout << std::endl;

	unsigned int const msTimeElapsed = (unsigned int)(((double)(clockEnd - clockBegin) / CLOCKS_PER_SEC) * 1000.0);
	std::cout << msTimeElapsed << std::endl;

    finIndex.close();
    finPosindex.close();
	return 0;
}
