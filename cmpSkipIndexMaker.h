#pragma once
#ifndef CMPSKIPINDEXMAKER_H
#define CMPSKIPINDEXMAKER_H

#include "compressedIndexMaker.h"

// ����� ��� ������������ ������� ������� � ����������� ��� ������� �� �������.
class CmpSkipIndexMaker: public CompressedIndexMaker
{
public:
    CmpSkipIndexMaker();
    virtual ~CmpSkipIndexMaker();
};

#endif
