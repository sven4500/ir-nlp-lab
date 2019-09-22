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

    // Метод возвращает константную ссылку на карту элементов для более быстрой
    // итерации когда требуется обработать всю карту последовательно.
    std::map<std::string, unsigned int> const& expose()const;

    // Метод возвращает количество встречемости строки str в корпусе.
    unsigned int operator[](std::string str)const;

    // Метод возвращает количество проиндексированных уникальных строк.
    unsigned int count()const;

    // Возвращает количество всего строк (включая повторяющиеся).
    unsigned int totalCount()const;

    // Метод возвращает набор из первых count часто встречаемых строк.
    std::vector<std::pair<std::string, unsigned int>> mostFrequent(unsigned int count)const;
    std::vector<std::pair<std::string, unsigned int>> leastFrequent(unsigned int count)const;

protected:
    // Предикат сортировки по ключу, т.е. по частоте встречаемости.
    template<typename ty1, typename ty2>
    static bool descendComp(std::pair<ty1, ty2> const& a, std::pair<ty1, ty2> const& b);

    template<typename ty1, typename ty2>
    static bool ascendComp(std::pair<ty1, ty2> const& a, std::pair<ty1, ty2> const& b);

    // Отображение для хранения пары <токен, количество>.
    std::map<std::string, unsigned int> _collection;
    unsigned int _totalCount;

};

#endif
