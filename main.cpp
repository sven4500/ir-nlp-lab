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
#include "TFIDFMaker.h"
#include "normIndexMaker.h"
#include "compressedIndexMaker.h"
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
			std::cout << "\rСтраниц обработано " << pages;

		#if defined(_DEBUG)
		if(pages == 2000)
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

	if(argc != 8)
	{
		std::cout << "IR3.exe _In_corpus.xml _In_tokens.xml _Out_index.dat _Out_posindex.dat _Out_tfidf.dat _Out_normindex.dat _Out_cmpindex.dat" << std::endl;
		return 1;
	}

	XMLDocument corpusDoc, tokensDoc;
	if(corpusDoc.LoadFile(argv[1]) != XML_SUCCESS || tokensDoc.LoadFile(argv[2]) != XML_SUCCESS)
	{
		std::cout << "Не удалось открыть XML файл." << std::endl;
		return 1;
	}

	std::cout << "Обрабатываю входные файлы: " << argv[1] << " и " << argv[2] << std::endl;

    /*IndexMaker* const maker[4] = {
        new InvertedIndexMaker(),
        new PositionalIndexMaker(),
        new TFIDFMaker(),
        new NormIndexMaker()
    };

    for(unsigned int i = 0; i < sizeof(maker) / sizeof(IndexMaker*); ++i)
    {

    }*/

    if(argv[3][0] != '0')
    {
        std::cout << std::endl << "Создаю инвертированный индекс..." << std::endl;

        InvertedIndexMaker maker;
        make(&tokensDoc, &maker);

        std::cout << "Средняя длина термина: " << maker.meanChars() << std::endl
            << "Количество терминов: " << maker.termCount() << std::endl;

        maker.write(argv[3]);
    }

    if(argv[4][0] != '0')
    {
        std::cout << std::endl << "Создаю координатный индекс..." << std::endl;

        PositionalIndexMaker maker;
        make(&corpusDoc, &maker);

        std::cout << "Количество токенов: " << maker.tokenCount() << std::endl
            << "Количество уникальных токенов: " << maker.uniqueTokenCount() << std::endl;

        maker.write(argv[4]);
    }

    if(argv[5][0] != '0')
    {
        std::cout << std::endl << "Создаю файл метрики TF-IDF..." << std::endl;

        TFIDFMaker maker;
        make(&corpusDoc, &maker);

        std::cout << "Количество документов: " << maker.docCount() << std::endl
            << "Количество токенов: " << maker.tokenCount() << std::endl;

        maker.write(argv[5]);
    }

    if(argv[6][0] != '0')
    {
        std::cout << std::endl << "Создаю нормализованный индекс..." << std::endl;
        NormIndexMaker maker;
        make(&tokensDoc, &maker);
        maker.write(argv[6]);
    }

    if(argv[7][0] != '0')
    {
        std::cout << std::endl << "Создаю сжатый индекс..." << std::endl;
        CompressedIndexMaker maker;
        make(&tokensDoc, &maker);
        maker.write(argv[7]);
    }

    return 0;
}
