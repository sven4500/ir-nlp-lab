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
			std::cout << "\r������� ���������� " << pages;

		#if defined(_DEBUG)
		if(pages == 2000)
			break;
		#endif
	}
    std::clock_t const timeEnd = clock();

    std::cout << std::endl << "������������ ���������� ����� ���������." << std::endl
        << "�����: " << (timeEnd - timeBegin) / CLOCKS_PER_SEC << " ���." << std::endl;
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
		std::cout << "�� ������� ������� XML ����." << std::endl;
		return 1;
	}

	std::cout << "����������� ������� �����: " << argv[1] << " � " << argv[2] << std::endl;

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
        std::cout << std::endl << "������ ��������������� ������..." << std::endl;

        InvertedIndexMaker maker;
        make(&tokensDoc, &maker);

        std::cout << "������� ����� �������: " << maker.meanChars() << std::endl
            << "���������� ��������: " << maker.termCount() << std::endl;

        maker.write(argv[3]);
    }

    if(argv[4][0] != '0')
    {
        std::cout << std::endl << "������ ������������ ������..." << std::endl;

        PositionalIndexMaker maker;
        make(&corpusDoc, &maker);

        std::cout << "���������� �������: " << maker.tokenCount() << std::endl
            << "���������� ���������� �������: " << maker.uniqueTokenCount() << std::endl;

        maker.write(argv[4]);
    }

    if(argv[5][0] != '0')
    {
        std::cout << std::endl << "������ ���� ������� TF-IDF..." << std::endl;

        TFIDFMaker maker;
        make(&corpusDoc, &maker);

        std::cout << "���������� ����������: " << maker.docCount() << std::endl
            << "���������� �������: " << maker.tokenCount() << std::endl;

        maker.write(argv[5]);
    }

    if(argv[6][0] != '0')
    {
        std::cout << std::endl << "������ ��������������� ������..." << std::endl;
        NormIndexMaker maker;
        make(&tokensDoc, &maker);
        maker.write(argv[6]);
    }

    if(argv[7][0] != '0')
    {
        std::cout << std::endl << "������ ������ ������..." << std::endl;
        CompressedIndexMaker maker;
        make(&tokensDoc, &maker);
        maker.write(argv[7]);
    }

    return 0;
}
