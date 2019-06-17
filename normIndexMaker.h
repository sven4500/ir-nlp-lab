#pragma once
#ifndef NORMINDEXMAKER_H
#define NORMINDEXMAKER_H

#include <string>
#include <map>
#include "invertedIndexMaker.h"

class NormIndexMaker: public InvertedIndexMaker
{
public:
    NormIndexMaker();
    ~NormIndexMaker();

    // Сравнивает две строки и находит количество совпадающих байт.
    static std::size_t equalSize(std::string const& s1, std::string const& s2);

    virtual bool update(tinyxml2::XMLElement const* elem);
    virtual bool write(std::string const& filename);

protected:
    void addTerm(std::string const& term, unsigned int docID);

    std::map<std::string, std::vector<unsigned int>> _termToDocID;
    unsigned int _;

};

#endif
