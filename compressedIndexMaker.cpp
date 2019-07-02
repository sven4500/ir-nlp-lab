#include <algorithm>
#include <fstream>
#include <iostream>
#include <cassert>
#include "compressedIndexMaker.h"

// ��������� ��������� ������� ������ ��� �������� v.
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

unsigned int bitCount(unsigned int value)
{
    // ������ �������� ��������� �������� ������ ���� ������ �������� �������
    // �������� �������� ������. ����� �������� ��� ������� � ����������.
    // ������� ��� �� �����, �� ��� �������� ����� ������ 1 ��� �������
    // ���������� bitCount ���������� ����� 1.
    value = near2(value);
    unsigned int bitCount = 1;
    while((value /= 2) && ++bitCount);
    return bitCount;
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

unsigned int CompressedIndexMaker::encodeNumber(unsigned int number, unsigned char* const buf)
{
    unsigned int bits = bitCount(number), bytes = 0;

    if(bits <= 6)
    {
        number |= 0 << 6;
        bytes = 1;
    }
    else if(bits <= 14)
    {
        number |= 1 << 14;
        bytes = 2;
    }
    else if(bits <= 22)
    {
        number |= 2 << 22;
        bytes = 3;
    }
    else if(bits <= 30)
    {
        number |= 3 << 30;
        bytes = 4;
    }

    // ������������ ����� � ����� � �������� �������. �������� �� ���������
    // ������� ������ (endianness).
    for(unsigned int i = 0; i < bytes; ++i)
        buf[i] = (number >> (8 * (bytes - i - 1))) & 0xff;

    return bytes;
}

bool CompressedIndexMaker::writeFile(std::string const& filename)
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

    // ����������� ���� �������.
    fout.seekp(termCount * 8, std::ios::cur);

    std::vector<std::pair<unsigned int, unsigned int>> dict;
    dict.resize(termCount);

    std::map<unsigned int, std::vector<unsigned int>>::iterator iter = _termToDocID.begin();
    for(std::size_t j = 0; j < _termToDocID.size(); ++j)
    {
        // ��������� ���������� � ������� � �������� � �������.
        dict[j].first = iter->first;
        dict[j].second = (unsigned int)fout.tellp();

        // ��������� ��������� ������������ ������ �����.
        #pragma pack(push, 1)
        struct
        {
            unsigned short _sign;
            unsigned short _blockBytes;
        }termHead = {0xABAB, 0};
        #pragma pack(pop)

        // ���������� ��������� ����� ��� ��� ���� �� ����� ������ �����.
        fout.seekp(sizeof(termHead), std::ios::cur);

        // ��������� �������������� � ������������ �������. ��� ����� ������
        // ��� ����� ������� ������ ����� ����������������.
        std::vector<unsigned int>& docID = iter->second;
        std::sort(docID.begin(), docID.end());

        for(std::size_t i = 0; i < docID.size(); ++i)
        {
            if(i != 0)
            {
                unsigned char idRaw[10] = {};
                unsigned int const id = docID[i] - docID[i-1];
                unsigned int bytes = encodeNumber(id, idRaw);

                // ��� ��� ������� ��������� ������� �� ���������� ��� ������
                // ���������� ���� ����� ���������� ������������ ��������.
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

        // ������ ����� ��� ���������� ����. ������ ����� ������ �����.
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

bool CompressedIndexMaker::write(std::string const& filename)
{
    /*std::cout << "����������� ������������� ���������: " << _docIDMin << std::endl
        << "������������ ������������� ���������: " << _docIDMax << std::endl;*/

    std::cout << "������ �� ������� 150 ����-����..." << std::endl;
    eraseStopWords(150);

    std::cout << "���� ����..." << std::endl;
    return writeFile(filename);
}
