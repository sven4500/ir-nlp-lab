#pragma once
#ifndef INDEXMAKER_H
#define INDEXMAKER_H

#include <string>
#include <tinyxml2.h>

class IndexMaker
{
public:
    virtual void clear()=0;

    virtual bool update(tinyxml2::XMLElement const* element)=0;
    virtual bool write(std::string const& filename)=0;

protected:
    IndexMaker(){}
    ~IndexMaker(){}

};

#endif
