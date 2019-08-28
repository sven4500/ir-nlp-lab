#pragma once
#ifndef ZONEINDEXMAKER_H
#define ZONEINDEXMAKER_H

#include "indexMaker.h"

class ZoneIndexMaker: public IndexMaker
{
public:
    ZoneIndexMaker();
    virtual ~ZoneIndexMaker();

    virtual void clear();

    virtual bool update(tinyxml2::XMLElement const* element);
    virtual bool write(std::string const& filename);

};

#endif
