#pragma once
#ifndef POSITIONALINDEXMAKER_H
#define POSITIONALINDEXMAKER_H

#include <map>
#include <vector>
#include "indexMaker.h"

class PositionalIndexMaker: public IndexMaker
{
public:
    PositionalIndexMaker();
    ~PositionalIndexMaker();

    virtual void clear();

    virtual bool update(tinyxml2::XMLElement const* element);
    virtual bool write(std::string const& filename);

    unsigned int tokenCount()const;
    unsigned int uniqueTokenCount()const;

protected:
    // ����������� ����������� ����� ������, �� ������ ������������.
    // ������������ ��������� � ��������������� ��������� ���� ��������
    // ������ � ��������������.
    std::map<unsigned int, std::vector<unsigned long long>> _tokenToPos;

    // ���������� ��������.
    unsigned int _tokenCount;
    unsigned int _uniqueTokenCount;

};

#endif
