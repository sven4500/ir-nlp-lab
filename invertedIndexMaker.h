#pragma once
#ifndef INVERTEDINDEXMAKER_H
#define INVERTEDINDEXMAKER_H

#include <map>
#include <vector>
#include "indexMaker.h"

class InvertedIndexMaker: public IndexMaker
{
public:
    InvertedIndexMaker();
    ~InvertedIndexMaker();

    virtual void clear();

    virtual bool update(tinyxml2::XMLElement const* element);
    virtual bool write(std::string const& filename);

    std::size_t termCount()const;
    std::size_t meanChars()const;

protected:
    std::map<unsigned int, std::vector<unsigned int>> _termToDocID;
    std::size_t _meanChars;

};

#endif
