#include <fstream>
#include <algorithm>
#include <vector>
#include <cstdlib> // rand
#include <iterator> // advance
#include "collector.h"
#include "textbeautifier.h"
using namespace tinyxml2;

template class Collector<unsigned int>;
template class Collector<double>;
template class Collector<float>;

template<typename T>
Collector<T>::Collector():
    _totalCount(0)
{}

template<typename T>
Collector<T>::~Collector()
{}

template<typename T>
void Collector<T>::clear()
{
    _collection.clear();
    _totalCount = 0;
}

template<typename T>
unsigned int Collector<T>::count()const
{
    return _collection.size();
}

template<typename T>
unsigned int Collector<T>::totalCount()const
{
    return _totalCount;
}

// https://stackoverflow.com/questions/19842035/how-can-i-sort-a-stdmap-first-by-value-then-by-key
template<typename T>
bool Collector<T>::descendComp(std::pair<std::string, T> const& a, std::pair<std::string, T> const& b)
{
    return (a.second != b.second) ? a.second > b.second : false;
}

template<typename T>
bool Collector<T>::ascendComp(std::pair<std::string, T> const& a, std::pair<std::string, T> const& b)
{
    return (a.second != b.second) ? a.second < b.second : false;
}

template<typename T>
std::map<std::string, T> const& Collector<T>::expose()const
{
    return _collection;
}

template<typename T>
T Collector<T>::operator[](std::string str)const
{
    auto const iter = _collection.find(str);
    return (iter != _collection.end()) ? iter->second : T();
}

template<typename T>
void Collector<T>::erase(unsigned int const count)
{
    for(unsigned int i = 0; i < count; ++i)
    {
        auto iter = _collection.begin();
        std::advance(iter, rand() % _collection.size());
        if(iter != _collection.end())
            _collection.erase(iter);
        else
            break;
    }
}

template<typename T>
void Collector<T>::update(XMLElement const* elem)
{
    // Исключаем ненужные нам символы UTF-8.
    std::string text = elem->GetText();
    wipe_all(text);

    std::size_t pos = 0;
    std::string token;

    while((pos = extract_token(text, token, pos)) != std::string::npos)
    {
        if(!is_alpha_numeric(token[0]))
        {
            to_lower_case(token);
            ++_collection[token];
            ++_totalCount;
        }
    }
}

template<typename T>
std::vector<std::pair<std::string, T>> Collector<T>::mostFrequent(unsigned int count)const
{
    std::vector<std::pair<std::string, T>> vect;

    count = std::min(_collection.size(), count);
    if(count == 0)
        return vect;

    vect.resize(_collection.size());

    unsigned int i = 0;
    for(auto iter = _collection.begin(), end = _collection.end(); iter != end; ++iter)
        vect[i++] = *iter;

    std::sort(vect.begin(), vect.end(), descendComp);
    vect.resize(count);
    return vect;
}

template<typename T>
std::vector<std::pair<std::string, T>> Collector<T>::leastFrequent(unsigned int count)const
{
    std::vector<std::pair<std::string, T>> vect;

    count = std::min(_collection.size(), count);
    if(count == 0)
        return vect;

    vect.resize(_collection.size());

    unsigned int i = 0;
    for(auto iter = _collection.begin(), end = _collection.end(); iter != end; ++iter)
        vect[i++] = *iter;

    std::sort(vect.begin(), vect.end(), ascendComp);
    vect.resize(count);
    return vect;
}

template<typename T>
bool Collector<T>::dump(char const* const filename)
{
    std::ofstream fout;
    fout.open(filename, std::ios::out | std::ios::trunc);
    if(!fout)
        return false;
    std::vector<std::pair<std::string, T>> const vect = mostFrequent(100);
    for(std::size_t i = 0; i < vect.size(); ++i)
        fout << vect[i].second << ": " << vect[i].first << "\n";
    fout.close();
    return true;
}
