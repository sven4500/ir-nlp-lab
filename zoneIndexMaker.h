#pragma once
#ifndef ZONEINDEXMAKER_H
#define ZONEINDEXMAKER_H

#include "positionalIndexMaker.h"

class ZoneIndexMaker: public PositionalIndexMaker
{
public:
    ZoneIndexMaker();
    virtual ~ZoneIndexMaker();

    virtual bool update(tinyxml2::XMLElement const* element);
    virtual bool write(std::string const& filename);

};

#endif
