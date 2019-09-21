#pragma once
#ifndef MONOGRAMCOLLECTOR_H
#define MONOGRAMCOLLECTOR_H

#include <vector>
#include <map>
#include <string>
#include "icorpusprocessor.h"

// Класс для сбора коллекции строк из XML элементов (документов корпуса).
// Хранит пары <строка, частота>. После наследования может быть использован для
// хранения как монограмм, так и биграмм.
class Collector: public ICorpusProcessor
{
public:
    Collector();
    virtual ~Collector();

    virtual void update(tinyxml2::XMLElement const* elem);
    virtual void clear();
    virtual bool dump(char const* filename);

    // Предикат сортировки по ключу, т.е. по частоте встречаемости.
    template<typename ty1, typename ty2>
    static bool comparer(std::pair<ty1, ty2> const& a, std::pair<ty1, ty2> const& b);

    // Метод возвращает количество встречемости строки str в корпусе.
    unsigned int operator[](std::string str)const;

    // Метод возвращает строку по порядковому номеру.
    std::string operator[](unsigned int i)const;

    // Метод возвращает количество проиндексированных уникальных строк.
    unsigned int count()const;

    // Метод возвращает набор из первых count часто встречаемых строк.
    std::vector<std::pair<std::string, unsigned int>> mostFrequent(unsigned int count)const;

protected:
    // Отображение для хранения пары <токен, количество>.
    std::map<std::string, unsigned int> _collection;

};

#endif
