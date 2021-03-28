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

bool CmpSkipIndexMaker::writeOne(std::ofstream& fout, unsigned int const number)
{
    unsigned char idRaw[4] = {};
    unsigned int bytes = encodeNumber(number, idRaw);

    if(bytes == 0)
        return false;

    fout.write((char*)&idRaw[0], bytes);
    return true;
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

        // Сортируем идентификаторы в возрастающем порядке. Это важно потому
        // что будем считать дельты между идентификаторами.
        std::vector<unsigned int>& docID = iter->second;
        std::sort(docID.begin(), docID.end());

        // Небольшой заголовок показывающий размер блока, и расстояние между
        // блоками ускорения (_stride).
        #pragma pack(push, 1)
        struct
        {
            unsigned short _sign;
            unsigned short _termID;
            unsigned short _stride;
            unsigned short _blockBytes;
        }termHead = {0xABAB, dict[j].first & 0xffff, (short)std::sqrt((float)docID.size()) + 1, 0};
        #pragma pack(pop)

        // Пропускаем заголовок блока так как пока не знаем размер блока.
        fout.seekp(sizeof(termHead), std::ios::cur);

        for(std::size_t i = 0, k = 0; i < docID.size(); ++i)
        {
            // Начальный и последний элемент списка не может быть
            // указателем прыжка. Последний указатель прыжка всегда
            // указывает на крайний элемент списка.
            if(k % termHead._stride == 0)
            {
                writeOne(fout, (i + termHead._stride < docID.size()) ? docID[i+termHead._stride] : docID.back());
                ++k;
            }

            writeOne(fout, (i != 0) ? docID[i] - docID[i-1] : docID[0]);
            ++k;
        }

        // Теперь знаем где закончился блок. Значит знаем размер блока.
        termHead._blockBytes = (unsigned int)fout.tellp() - dict[j].second;

        fout.seekp(dict[j].second, std::ios::beg);
        fout.write((char*)&termHead, sizeof(termHead));
        fout.seekp(0, std::ios::end);

        if(j % 10000 == 0)
            std::cout << '\r' << j << '/' << _termToDocID.size();

        ++iter;
    }

    std::cout << std::endl;

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
