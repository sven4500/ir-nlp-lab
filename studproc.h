#pragma once
#ifndef STUDPROC_H
#define STUDPROC_H

#include "collector.h"
#include "bigramcollector.h"

class StudProc: public Collector
{
public:
    StudProc();
    virtual ~StudProc();

    void update(Collector const& monograms, BigramCollector const& bigrams);

};

#endif
