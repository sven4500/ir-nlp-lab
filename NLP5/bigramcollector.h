#pragma once
#ifndef BIGRAMCOLLECTOR_H
#define BIGRAMCOLLECTOR_H

#include "collector.h"

template<typename T>
class BigramCollector: public Collector<T>
{
public:
    BigramCollector();
    virtual ~BigramCollector();

    // Методы запоминают наиболее часто употребляемые токены и исключают
    // формирование биграмм которые их содержат.
    void rememberMostFrequent(std::vector<std::pair<std::string, T>> const& frequent);
    void rememberMostFrequent(std::map<std::string, T> const& freqent);

    virtual void update(tinyxml2::XMLElement const* elem);

protected:
    std::map<std::string, T> _mostFrequent;

};

#endif
