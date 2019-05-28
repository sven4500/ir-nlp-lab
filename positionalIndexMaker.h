#pragma once
#ifndef POSITIONALINDEXMAKER_H
#define POSITIONALINDEXMAKER_H

#include "indexMaker.h"

class PositionalIndexMaker: public IndexMaker
{
public:
    PositionalIndexMaker();
    ~PositionalIndexMaker();

    virtual void clear();

    virtual bool update(tinyxml2::XMLElement const* element);
    virtual bool write(std::string const& filename);

};

#endif
