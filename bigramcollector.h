#pragma once
#ifndef BIGRAMCOLLECTOR_H
#define BIGRAMCOLLECTOR_H

#include "collector.h"

class BigramCollector: public Collector
{
public:
    BigramCollector();
    virtual ~BigramCollector();

    // Методы запоминают наиболее часто употребляемые токены и исключают
    // формирование биграмм которые их содержат.
    void rememberMostFrequent(std::vector<std::pair<std::string, unsigned int>> const& frequent);
    void rememberMostFrequent(std::map<std::string, unsigned int> const& freqent);

    virtual void update(tinyxml2::XMLElement const* elem);

protected:
    std::map<std::string, unsigned int> _mostFrequent;

};

#endif
