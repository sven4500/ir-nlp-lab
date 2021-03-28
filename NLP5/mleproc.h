#pragma once
#ifndef MLEPROC_H
#define MLEPROC_H

#include "collector.h"
#include "bigramcollector.h"

template<typename T>
class MLEProc: public Collector<T>
{
public:
    MLEProc();
    virtual ~MLEProc();

    static T logL(T k, T n, T x);

    virtual bool dump(char const* filename);

    void update(Collector<unsigned int> const& monograms, BigramCollector<unsigned int> const& bigrams);

};

#endif
