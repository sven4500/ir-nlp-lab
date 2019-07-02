#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include "cmpSkipIndexMaker.h"

CmpSkipIndexMaker::CmpSkipIndexMaker()
{

}

CmpSkipIndexMaker::~CmpSkipIndexMaker()
{

}

bool CmpSkipIndexMaker::writeFile(std::string const& filename)
{
    std::ofstream fout;
    fout.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    unsigned int const termCount = _termToDocID.size();

    struct
    {
        unsigned int _sign;
        unsigned int _termCount;
        unsigned int _unused[2];
    }fileHead = {0xDFDFDFDF, termCount, 0, 0};

    fout.write((char*)&fileHead, sizeof(fileHead));

    // Пропускаяем пока словарь.
    fout.seekp(termCount * 8, std::ios::cur);

    std::vector<std::pair<unsigned int, unsigned int>> dict;
    dict.resize(termCount);

    std::map<unsigned int, std::vector<unsigned int>>::iterator iter = _termToDocID.begin();
    for(std::size_t j = 0; j < _termToDocID.size(); ++j)
    {
        // Сохраняем информацию о термине и смещении в словарь.
        dict[j].first = iter->first;
        dict[j].second = (unsigned int)fout.tellp();

        // Небольшой заголовок показывающий размер блока.
        #pragma pack(push, 1)
        struct
        {
            unsigned short _sign;
            unsigned short _stride;
            unsigned int _blockBytes;
        }termHead = {0xABAB, 0, 0};
        #pragma pack(pop)

        // Пропускаем заголовок блока так как пока не знаем размер блока.
        fout.seekp(sizeof(termHead), std::ios::cur);

        // Сортируем идентификаторы в возрастающем порядке. Это важно потому
        // что будем считать дельты между идентификаторами.
        std::vector<unsigned int>& docID = iter->second;
        std::sort(docID.begin(), docID.end());

        for(std::size_t i = 0; i < docID.size(); ++i)
        {
            if(i != 0)
            {
                unsigned char idRaw[10] = {};
                unsigned int const id = docID[i] - docID[i-1];
                unsigned int bytes = encodeNumber(id, idRaw);

                // Так как текущий результат зависит от предыдущий нет смысла
                // продолжать если вдруг невозможно закодировать значение.
                if(bytes == 0)
                    break;

                fout.write((char*)&idRaw[0], bytes);
            }
            else
            {
                unsigned int id = docID[0];
                fout.write((char*)&id, 4);
            }
        }

        // Теперь знаем где закончился блок. Значит знаем размер блока.
        termHead._blockBytes = (unsigned int)fout.tellp() - dict[j].second;

        fout.seekp(dict[j].second, std::ios::beg);
        fout.write((char*)&termHead, sizeof(termHead));
        fout.seekp(0, std::ios::end);

        ++iter;
    }

    fout.seekp(16, std::ios::beg);
    fout.write((char*)&dict[0], termCount * 8);

    fout.close();
    return true;
}

bool CmpSkipIndexMaker::write(std::string const& filename)
{
    std::cout << "Удаляю из индекса 150 стоп-слов..." << std::endl;
    eraseStopWords(150);

    std::cout << "Пишу файл..." << std::endl;
    return writeFile(filename);
}
