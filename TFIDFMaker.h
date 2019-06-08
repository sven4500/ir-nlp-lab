#pragma once
#ifndef TFIDFMAKER_H
#define TFIDFMAKER_H

#include <map>
#include <vector>
#include "indexMaker.h"

class TFIDFMaker: public IndexMaker
{
public:
    TFIDFMaker();
    ~TFIDFMaker();

    virtual void clear();

    virtual bool update(tinyxml2::XMLElement const* element);
    virtual bool write(std::string const& filename);

    unsigned int getDocCount()const;
    unsigned int getTokenCount()const;

protected:
    struct DocFreq
    {
        DocFreq(): _docID(0), _tokenCount(0), _tokenTotalCount(0)
        {}

        unsigned int _docID;
        unsigned short _tokenCount;
        unsigned short _tokenTotalCount;
    };

    // ����������� �� ������ �� ������ ���������� ��� ���� ����� �����������.
    // ������ ������� ���������� ���������� ���������� � ������� �����������
    // ����� (IDF).
    std::map<unsigned int, std::vector<DocFreq>> _tok2DocFreq;

    // ���������� ���������� �������.
    unsigned int _docCount;

};

#endif
