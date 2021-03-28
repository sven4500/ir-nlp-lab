#pragma once
#ifndef COMPRESSEDINDEXMAKER_H
#define COMPRESSEDINDEXMAKER_H

#include "invertedIndexMaker.h"

class CompressedIndexMaker: public InvertedIndexMaker
{
public:
    virtual bool write(std::string const& filename);

protected:
    // ����� �������� ���� ����� number. ����� buf ������ ������� ������ 4
    // �����. ����� ���������� ���������� ���� ��� ��������.
    static unsigned int encodeNumber(unsigned int number, unsigned char* buf);

    // ����� ���������� ��������� ������ ���������� � ������� � �������
    // ����������. ������� �� ��������� ��������� �� ������:
    // https://stackoverflow.com/questions/466204/rounding-up-to-next-power-of-2#
    static unsigned int near2(unsigned int number);

    // ����� ������������ ���������� ���, ������� �������� �����.
    static unsigned int bitCount(unsigned int number);

    void eraseStopWords(unsigned int numStopWords);
    bool writeFile(std::string const& filename);

};

#endif
