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
    // Только на время отладки всплывает окно которое останавливает работу
	// приложения пока пользователь не нажмёт кнопку OK. Это даст нам время
	// чтобы подцепить отладчик к программе. Это вынужденная мера которая
	// возникла потому что консоль каверкает строку зароса.
    MessageBox(NULL, NULL, NULL, MB_OK);
    #endif

    unsigned int const numFiles = 4;
    std::ifstream fin[numFiles];
    for(unsigned int i = 0; i < numFiles; ++i)
    {
        fin[i].open(argv[i+1], std::ios::in | std::ios::binary);
        if(!fin[i])
        {
            std::cout << "Не могу открыть один или несколько файлов." << std::endl;
            closeFiles(fin, numFiles);
            return -1;
        }
    }

    // Из стандартного потока ввода получаем строку содержащую запрос.
	std::string query;
	std::getline(std::cin, query);

    // Замеряем сколько времени потратили на разбор запроса. Разбор запроса
    // включает одновременно ранжирование.
	std::clock_t const clockBegin = clock();
	std::vector<unsigned int> const docID = parse(query.c_str(), fin[0], fin[1], fin[2], fin[3]);
	std::clock_t const clockEnd = clock();

    // В стандартный поток вывода сообщаем идентификаторы документов
    // удовлетворяющих запросу.
	for(std::size_t i = 0; i < docID.size(); ++i)
		std::cout << docID[i] << ' ';
	std::cout << std::endl;

    // Сообщаем среднюю длину одного прыжка по индексу.
    double const skipMean = (double)skip / skipCount;
    std::cout << skipMean << std::endl;

    // Сообщаем затраченное на обработку воемя.
	unsigned int const msTimeElapsed = (unsigned int)(((double)(clockEnd - clockBegin) / CLOCKS_PER_SEC) * 1000.0);
	std::cout << msTimeElapsed << std::endl;

    // Закрываем все открытые файлы индексов.
    //_runaway:
    closeFiles(fin, numFiles);

	return 0;
}
