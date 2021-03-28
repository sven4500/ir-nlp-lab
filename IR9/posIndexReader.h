#pragma once
#ifndef POSINDEXREADER_H
#define POSINDEXREADER_H

#include <fstream>
#include <vector>

class PosIndexReader
{
public:
    // Функция возвращает отображение документов на список словопозиций для
    // термина termID.
    static std::vector<std::pair<unsigned int, std::vector<unsigned int>>> getDocIDToPos(unsigned int tokenID, std::ifstream& fin);

protected:
    PosIndexReader();
    ~PosIndexReader();

};

#endif
