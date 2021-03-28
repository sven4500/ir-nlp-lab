#pragma once
#ifndef COMPRESSEDINDEXMAKER_H
#define COMPRESSEDINDEXMAKER_H

#include "invertedIndexMaker.h"

class CompressedIndexMaker: public InvertedIndexMaker
{
public:
    virtual bool write(std::string const& filename);

protected:
    // Метод кодирует одно число number. Буфер buf должен вмещать хотябы 4
    // байта. Метод возвращает количество байт для хранения.
    static unsigned int encodeNumber(unsigned int number, unsigned char* buf);

    // Метод возвращает ближайшую двойку возведённую в степень в сторону
    // увеличения. Найдено на просторах интернета по адресу:
    // https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2#
    static unsigned int near2(unsigned int number);

    // Метод подсчитывает количество бит, которые занимает число.
    static unsigned int bitCount(unsigned int number);

    void eraseStopWords(unsigned int numStopWords);
    bool writeFile(std::string const& filename);

};

#endif
