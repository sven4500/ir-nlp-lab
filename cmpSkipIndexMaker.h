#pragma once
#ifndef CMPSKIPINDEXMAKER_H
#define CMPSKIPINDEXMAKER_H

#include "compressedIndexMaker.h"

// Класс для формирования сжатого индекса с подсказками для прыжков по индексу.
class CmpSkipIndexMaker: public CompressedIndexMaker
{
public:
    CmpSkipIndexMaker();
    virtual ~CmpSkipIndexMaker();
};

#endif
