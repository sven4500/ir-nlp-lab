#include <fstream>
#include <algorithm>
#include <vector>
#include "collector.h"
#include "textbeautifier.h"
using namespace tinyxml2;

Collector::Collector():
    _totalCount(0)
{}

Collector::~Collector()
{}

void Collector::clear()
{
    _collection.clear();
    _totalCount = 0;
}

unsigned int Collector::count()const
{
    return _collection.size();
}

unsigned int Collector::totalCount()const
{
    return _totalCount;
}

// https://stackoverflow.com/questions/19842035/how-can-i-sort-a-stdmap-first-by-value-then-by-key
template<typename ty1, typename ty2>
bool Collector::descendComp(std::pair<ty1, ty2> const& a, std::pair<ty1, ty2> const& b)
{
    return (a.second != b.second) ? a.second > b.second : false;
}

template<typename ty1, typename ty2>
bool Collector::ascendComp(std::pair<ty1, ty2> const& a, std::pair<ty1, ty2> const& b)
{
    return (a.second != b.second) ? a.second < b.second : false;
}

std::map<std::string, unsigned int> const& Collector::expose()const
{
    return _collection;
}

unsigned int Collector::operator[](std::string str)const
{
    auto const iter = _collection.find(str);
    return (iter != _collection.end()) ? iter->second : 0;
}

void Collector::update(XMLElement const* elem)
{
    // ��������� �������� ��� ������� UTF-8.
    std::string text = elem->GetText();
    wipe_all(text);

    std::size_t pos = 0;
    std::string token;

    while((pos = extract_token(text, token, pos)) != std::string::npos)
    {
        if(/*!token.empty() &&*/ !is_alpha_numeric(token[0]))
        {
            to_lower_case(token);
            ++_collection[token];
            ++_totalCount;
        }
    }
}

std::vector<std::pair<std::string, unsigned int>> Collector::mostFrequent(unsigned int count)const
{
    std::vector<std::pair<std::string, unsigned int>> vect;

    count = std::min(_collection.size(), count);
    if(count == 0)
        return vect;

    vect.resize(_collection.size());

    unsigned int i = 0;
    for(auto iter = _collection.begin(), end = _collection.end(); iter != end; ++iter)
        vect[i++] = *iter;

    std::sort(vect.begin(), vect.end(), descendComp<std::string, unsigned int>);
    vect.resize(count);
    return vect;
}

std::vector<std::pair<std::string, unsigned int>> Collector::leastFrequent(unsigned int count)const
{
    std::vector<std::pair<std::string, unsigned int>> vect;

    count = std::min(_collection.size(), count);
    if(count == 0)
        return vect;

    vect.resize(_collection.size());

    unsigned int i = 0;
    for(auto iter = _collection.begin(), end = _collection.end(); iter != end; ++iter)
        vect[i++] = *iter;

    std::sort(vect.begin(), vect.end(), ascendComp<std::string, unsigned int>);
    vect.resize(count);
    return vect;
}

bool Collector::dump(char const* const filename)
{
    std::ofstream fout;
    fout.open(filename, std::ios::out | std::ios::trunc);
    if(!fout)
        return false;
    std::vector<std::pair<std::string, unsigned int>> const vect = mostFrequent(100);
    for(std::size_t i = 0; i < vect.size(); ++i)
        fout << vect[i].second << ": " << vect[i].first << "\n";
    fout.close();
    return true;
}
