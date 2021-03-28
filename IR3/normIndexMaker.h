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

    virtual bool update(tinyxml2::XMLElement const* elem);

protected:
    void addTokenAsTerm(std::string const& token, unsigned int docID);

};

#endif
