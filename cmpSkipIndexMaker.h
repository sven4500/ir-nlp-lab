#pragma once
#ifndef CMPSKIPINDEXMAKER_H
#define CMPSKIPINDEXMAKER_H

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
    bool writeFile(std::string const& filename);

};

#endif
