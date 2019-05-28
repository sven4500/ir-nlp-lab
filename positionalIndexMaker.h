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
    // Отображение контрольной суммы токена, на другое отображение,
    // которое отображает идентификатор документа на вектор координат.
    std::map<unsigned int, std::map<unsigned int, std::vector<unsigned short>>> _tokenToPos;

};

#endif
