#include <fstream>
#include <iostream>
#include "compressedIndexMaker.h"

// Вычисляем ближайшую степерь двойки для значения v.
// https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2#
unsigned int near2(unsigned int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

void CompressedIndexMaker::eraseStopWords(unsigned int const numStopWords)
{
    for(unsigned int i = 0; i < numStopWords; ++i)
    {
        std::map<unsigned int, std::vector<unsigned int>>::iterator maxIter = _termToDocID.begin();
        for(std::map<unsigned int, std::vector<unsigned int>>::iterator iter = _termToDocID.begin(), end = _termToDocID.end(); iter != end; ++iter)
            if(iter->second.size() > maxIter->second.size())
                maxIter = iter;
        if(maxIter != _termToDocID.end())
            _termToDocID.erase(maxIter);
    }
}

bool CompressedIndexMaker::writeFile(std::string const& filename)
{
    std::ofstream fout;
    fout.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    unsigned int docIDBits = 0;
    {
        // Вычисляем количество бит на хранение одного идентификатора
        // документа.
        unsigned int docIDPower = near2(_docIDMax - _docIDMin);
        while((docIDPower /= 2) && ++docIDBits);
        docIDBits = ((docIDBits + 1) / 2) * 2;
    }

    std::cout << "Минимальный идентификатор документа: " << _docIDMin << std::endl
        << "Максимальный идентификатор документа: " << _docIDMax << std::endl
        << "Битность идентификатора: " << docIDBits << std::endl;

    struct
    {
        unsigned int _sign;
        unsigned int _termCount;
        unsigned int _docIDBits;
        unsigned int _unused;
    }fileHead = {0xDFDFDFDF, _termToDocID.size(), docIDBits, 0};

    fout.close();
    return false;
}

bool CompressedIndexMaker::write(std::string const& filename)
{
    std::cout << "Удаляю из индекса 150 стоп-слов..." << std::endl;
    //eraseStopWords(150);

    std::cout << "Пишу файл..." << std::endl;
    return writeFile(filename);
}
