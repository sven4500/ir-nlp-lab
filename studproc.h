#pragma once
#ifndef STUDPROC_H
#define STUDPROC_H

#include "collector.h"
#include "bigramcollector.h"

class StudProc: public Collector<double>
{
public:
    StudProc();
    virtual ~StudProc();

    virtual bool dump(char const* filename);

    void update(Collector<unsigned int> const& monograms, BigramCollector<unsigned int> const& bigrams);

};

#endif
