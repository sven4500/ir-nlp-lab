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

    // Метод возвращает количество проиндексированных уникальных токенов.
    unsigned int tokenCount()const;

    // Метод возвращает набор из первых count часто встречаемых токенов.
    std::vector<std::pair<std::string, unsigned int>> mostFrequent(unsigned int count)const;

protected:
    // Отображение для хранения пары <токен, количество>.
    std::map<std::string, unsigned int> _collection;

};

#endif
