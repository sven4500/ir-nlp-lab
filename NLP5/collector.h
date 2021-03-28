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
template<typename T>
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
    std::map<std::string, T> const& expose()const;

    // Удаляет count произвольных элементов из списка.
    void erase(unsigned int count);

    // Метод возвращает количество встречемости строки str в корпусе.
    T operator[](std::string str)const;

    // Метод возвращает количество проиндексированных уникальных строк.
    unsigned int count()const;

    // Возвращает количество всего строк (включая повторяющиеся).
    unsigned int totalCount()const;

    // Метод возвращает набор из первых count часто встречаемых строк.
    std::vector<std::pair<std::string, T>> mostFrequent(unsigned int count)const;
    std::vector<std::pair<std::string, T>> leastFrequent(unsigned int count)const;

protected:
    // Предикат сортировки по ключу, т.е. по частоте встречаемости.
    static bool descendComp(std::pair<std::string, T> const& a, std::pair<std::string, T> const& b);
    static bool ascendComp(std::pair<std::string, T> const& a, std::pair<std::string, T> const& b);

    // Отображение для хранения пары <токен, количество>.
    std::map<std::string, T> _collection;
    unsigned int _totalCount;

};

#endif
