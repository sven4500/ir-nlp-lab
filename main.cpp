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

	std::ifstream finInd(argv[1], std::ios::in | std::ios::binary),
        finPosInd(argv[2], std::ios::in | std::ios::binary),
        finTFIDF(argv[3], std::ios::in | std::ios::binary),
        finZoneInd(argv[4], std::ios::in | std::ios::binary);

	if(!finInd || !finPosInd || !finTFIDF || !finZoneInd)
	{
		std::cout << "Не могу открыть один или несколько файлов." << std::endl;
		return -1;
	}

    // Из стандартного потока ввода получаем строку содержащую запрос.
	std::string query;
	std::getline(std::cin, query);

    // Замеряем сколько времени потратили на разбор запроса. Разбор запроса
    // включает одновременно ранжирование.
	std::clock_t const clockBegin = clock();
	std::vector<unsigned int> const docID = parse(finInd, finPosInd, finTFIDF, query.c_str());
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
    finInd.close();
    finPosInd.close();
    finTFIDF.close();
    finZoneInd.close();

	return 0;
}
