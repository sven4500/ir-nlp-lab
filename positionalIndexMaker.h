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

protected:
    // ����������� ����������� ����� ������, �� ������ ������������.
    // ������������ ��������� � ��������������� ��������� ���� ��������
    // ������ � ��������������.
    std::map<unsigned int, std::vector<unsigned long long>> _tokenToPos;

};

#endif
