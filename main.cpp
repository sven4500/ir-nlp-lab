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

inline void closeFiles(std::ifstream* fin, unsigned int count)
{
    while(count--)
        (fin++)->close();
}

int main(int argc, char** argv)
{
	std::setlocale(LC_CTYPE, "Russian");

	if(argc != 5)
	{
		std::cout << "IR9.exe _In_CmpSkipIndex.dat _In_PosIndex.dat _In_TFIDF.dat _In_ZoneIndex.dat" << std::endl;
		return 1;
	}

    #ifdef _DEBUG
    // ������ �� ����� ������� ��������� ���� ������� ������������� ������
	// ���������� ���� ������������ �� ����� ������ OK. ��� ���� ��� �����
	// ����� ��������� �������� � ���������. ��� ����������� ���� �������
	// �������� ������ ��� ������� ��������� ������ ������.
    MessageBox(NULL, NULL, NULL, MB_OK);
    #endif

    unsigned int const numFiles = 4;
    std::ifstream fin[numFiles];
    for(unsigned int i = 0; i < numFiles; ++i)
    {
        fin[i].open(argv[i+1], std::ios::in | std::ios::binary);
        if(!fin[i])
        {
            std::cout << "�� ���� ������� ���� ��� ��������� ������." << std::endl;
            closeFiles(fin, numFiles);
            return -1;
        }
    }

    // �� ������������ ������ ����� �������� ������ ���������� ������.
	std::string query;
	std::getline(std::cin, query);

    // �������� ������� ������� ��������� �� ������ �������. ������ �������
    // �������� ������������ ������������.
	std::clock_t const clockBegin = clock();
	std::vector<unsigned int> const docID = parse(query.c_str(), fin[0], fin[1], fin[2], fin[3]);
	std::clock_t const clockEnd = clock();

    // � ����������� ����� ������ �������� �������������� ����������
    // ��������������� �������.
	for(std::size_t i = 0; i < docID.size(); ++i)
		std::cout << docID[i] << ' ';
	std::cout << std::endl;

    // �������� ������� ����� ������ ������ �� �������.
    double const skipMean = (double)skip / skipCount;
    std::cout << skipMean << std::endl;

    // �������� ����������� �� ��������� �����.
	unsigned int const msTimeElapsed = (unsigned int)(((double)(clockEnd - clockBegin) / CLOCKS_PER_SEC) * 1000.0);
	std::cout << msTimeElapsed << std::endl;

    // ��������� ��� �������� ����� ��������.
    //_runaway:
    closeFiles(fin, numFiles);

	return 0;
}
