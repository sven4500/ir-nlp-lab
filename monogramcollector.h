#pragma once
#ifndef MONOGRAMCOLLECTOR_H
#define MONOGRAMCOLLECTOR_H

#include <vector>
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

    template<typename ty1, typename ty2>
    static bool comparer(std::pair<ty1, ty2> const& a, std::pair<ty1, ty2> const& b);

    // ����� ���������� ���������� ������������������ ���������� �������.
    unsigned int tokenCount()const;

    // ����� ���������� ����� �� ������ count ����� ����������� �������.
    std::vector<std::pair<std::string, unsigned int>> mostFrequent(unsigned int count)const;

protected:
    // ����������� ��� �������� ���� <�����, ����������>.
    std::map<std::string, unsigned int> _collection;

};

#endif
