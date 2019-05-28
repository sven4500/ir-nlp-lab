#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <ctime>
#include <tinyxml2.h>
#include <crc32.h>
#include "indexMaker.h"
#include "invertedIndexMaker.h"
#include "positionalIndexMaker.h"
using namespace tinyxml2;

void make(XMLDocument* doc, IndexMaker* maker)
{
    unsigned int pages = 0;

	XMLElement const* root = doc->FirstChildElement();
	XMLElement const* pageElem = (root) ? root->FirstChildElement() : 0;

    std::clock_t const timeBegin = clock();
	while(pageElem != 0)
	{
		maker->update(pageElem);
		pageElem = pageElem->NextSiblingElement();
		++pages;

		if(pages % 500 == 0)
			std::cout << "\rСтатей обработано: " << pages;

		#if defined(_DEBUG)
		if(pages == 1000)
			break;
		#endif
	}
    std::clock_t const timeEnd = clock();

    std::cout << std::endl << "Формирование индексного файла завершено." << std::endl
        << "Время: " << (timeEnd - timeBegin) / CLOCKS_PER_SEC << " сек." << std::endl;
}

int main(int argc, char** argv)
{
	setlocale(LC_CTYPE, "Russian");

	if(argc != 5)
	{
		std::cout << "IR3.exe _In_corpus.xml _In_tokens.xml _Out_index.dat _Out_posindex.dat" << std::endl;
		return 1;
	}

	XMLDocument corpusDoc, tokensDoc;
	if(corpusDoc.LoadFile(argv[1]) != XML_SUCCESS || tokensDoc.LoadFile(argv[2]) != XML_SUCCESS)
	{
		std::cout << "Не удалось открыть XML файл." << std::endl;
		return 1;
	}

	std::cout << "Обрабатываю файлы: " << argv[1] << " и " << argv[2] << std::endl;

    /*{
        std::cout << std::endl << "Создаю инвертированный индекс..." << std::endl;

        InvertedIndexMaker maker;
        make(&tokensDoc, &maker);

        std::cout << "Средняя длина термина: " << maker.meanChars() << std::endl
            << "Количество терминов: " << maker.termCount() << std::endl;

        maker.write(argv[3]);
    }*/

    {
        std::cout << std::endl << "Создаю позиционный индекс..." << std::endl;

        PositionalIndexMaker maker;
        make(&corpusDoc, &maker);

        maker.write(argv[4]);
    }

    return 0;
}
