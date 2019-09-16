#pragma once
#ifndef MONOGRAMCOLLECTOR_H
#define MONOGRAMCOLLECTOR_H

#include <map>
#include <string>
#include "icorpusprocessor.h"

class MonogramCollector: public ICorpusProcessor
{
public:
    MonogramCollector();
    virtual ~MonogramCollector();

    virtual void update(tinyxml2::XMLElement const* elem);
    virtual void clear();
    virtual bool dump(char const* filename);

protected:
    // ����������� ��� �������� ���� <�����, ����������>.
    std::map<std::string, unsigned int> _tokens;
    unsigned int _numTokens;

};

#endif
