#pragma once
#ifndef STUDPROC_H
#define STUDPROC_H

#include "collector.h"
#include "bigramcollector.h"

template<typename T>
class StudProc: public Collector<T>
{
public:
    StudProc();
    virtual ~StudProc();

    virtual bool dump(char const* filename);

    void update(Collector<unsigned int> const& monograms, BigramCollector<unsigned int> const& bigrams);

};

#endif
