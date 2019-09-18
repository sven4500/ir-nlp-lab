#pragma once
#ifndef BIGRAMCOLLECTOR_H
#define BIGRAMCOLLECTOR_H

#include "monogramcollector.h"

class BigramCollector: public MonogramCollector
{
public:
    BigramCollector();
    virtual ~BigramCollector();

    void tellMostFrequent(std::vector<std::pair<std::string, unsigned int>> const& frequent);
    void tellMostFrequent(std::map<std::string, unsigned int> const& freqent);

    virtual void update(tinyxml2::XMLElement const* elem);

protected:
    std::map<std::string, unsigned int> _mostFrequent;

};

#endif
