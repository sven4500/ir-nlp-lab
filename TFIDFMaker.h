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
        unsigned int _docID;
        unsigned short _tokenCount;
        unsigned short _tokenTotalCount;
    };

    // ����������� ��������� �� ������ � ����������� ���������� � ������� ��
    // �����������.
    std::map<unsigned int, unsigned int> _tok2CorpFreq;

    // ����������� �� ������ �� ������ ���������� � ������� ���� �����
    // �����������.
    std::map<unsigned int, std::vector<DocFreq>> _tok2DocFreq;

    // ���������� ���������� �������.
    unsigned int _docCount;

};

#endif
