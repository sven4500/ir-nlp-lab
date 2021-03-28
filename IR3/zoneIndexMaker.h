#pragma once
#ifndef ZONEINDEXMAKER_H
#define ZONEINDEXMAKER_H

#include "positionalIndexMaker.h"

class ZoneIndexMaker: public PositionalIndexMaker
{
public:
    // Количество зон на которые разбивается текст каждого документа. По
    // словопозиции определяется принадлежность токена к той или иной зоне.
    static unsigned int const _numberOfZones = 2;

    ZoneIndexMaker();
    virtual ~ZoneIndexMaker();

    virtual bool update(tinyxml2::XMLElement const* element);
    virtual bool write(std::string const& filename);

};

#endif
