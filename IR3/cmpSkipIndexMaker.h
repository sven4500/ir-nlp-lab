#pragma once
#ifndef CMPSKIPINDEXMAKER_H
#define CMPSKIPINDEXMAKER_H

#include <fstream>
#include <string>
#include "compressedIndexMaker.h"

// ����� ��� ������������ ������� ������� � ����������� ��� ������� �� �������.
class CmpSkipIndexMaker: public CompressedIndexMaker
{
public:
    CmpSkipIndexMaker();
    virtual ~CmpSkipIndexMaker();

    virtual bool write(std::string const& filename);

protected:
    static bool writeOne(std::ofstream& fout, unsigned int const number);

    bool writeFile(std::string const& filename);

};

#endif
