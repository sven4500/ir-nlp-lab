#pragma once
#ifndef NORMINDEXMAKER_H
#define NORMINDEXMAKER_H

#include <string>
#include <map>
//#include <set>
#include "invertedIndexMaker.h"

class NormIndexMaker: public InvertedIndexMaker
{
public:
    NormIndexMaker();
    ~NormIndexMaker();

    // ���������� ��� ������ � ������� ���������� ����������� ����. ����������
    // ���� ���������� ������ ������� ����.
    static std::size_t equalSize(std::string const& s1, std::string const& s2);

    virtual bool update(tinyxml2::XMLElement const* elem);
    virtual bool write(std::string const& filename);

protected:
    void addTokenAsTerm(std::string const& token, unsigned int docID);

    std::map<std::string, std::vector<unsigned int>> _termToDocID;

};

#endif
