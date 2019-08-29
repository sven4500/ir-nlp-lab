#pragma once
#ifndef POSITIONALINDEXMAKER_H
#define POSITIONALINDEXMAKER_H

#include <map>
#include <vector>
#include <string>
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

    static unsigned int getTokenID(std::string const& text, std::size_t beg, std::size_t& end);

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
