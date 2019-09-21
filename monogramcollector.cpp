#include <fstream>
#include <algorithm>
#include <vector>
#include "monogramcollector.h"
#include "textbeautifier.h"
using namespace tinyxml2;

MonogramCollector::MonogramCollector()
{}

MonogramCollector::~MonogramCollector()
{}

void MonogramCollector::clear()
{}

unsigned int MonogramCollector::count()const
{
    return _collection.size();
}

// https://stackoverflow.com/questions/19842035/how-can-i-sort-a-stdmap-first-by-value-then-by-key
template<typename ty1, typename ty2>
bool MonogramCollector::comparer(std::pair<ty1, ty2> const& a, std::pair<ty1, ty2> const& b)
{
    return (a.second != b.second) ? a.second > b.second : false;
}

unsigned int MonogramCollector::operator[](std::string str)const
{
    //to_lower_case(str);
    auto const iter = _collection.find(str);
    return (iter != _collection.end()) ? iter->second : 0;
}

std::string MonogramCollector::operator[](unsigned int const i)const
{
    auto iter = _collection.begin(), end = _collection.end();
    for(unsigned int j = 0; j < i || iter != end; ++j)
        ++iter;
    return (iter != end) ? iter->first : std::string();
}

void MonogramCollector::update(XMLElement const* elem)
{
    // ��������� �������� ��� ������� UTF-8.
    std::string text = elem->GetText();
    wipe_all(text);

    std::size_t pos = 0;
    std::string token;

    while((pos = extract_token(text, token, pos)) != std::string::npos)
    {
        if(!token.empty() && !is_alpha_numeric(token[0]))
        {
            to_lower_case(token);
            _collection[token]++;
        }
    }
}

std::vector<std::pair<std::string, unsigned int>> MonogramCollector::mostFrequent(unsigned int count)const
{
    std::vector<std::pair<std::string, unsigned int>> vect;

    count = std::min(_collection.size(), count);
    if(count == 0)
        return vect;

    vect.resize(_collection.size());

    unsigned int i = 0;
    for(auto iter = _collection.begin(), end = _collection.end(); iter != end; ++iter)
        vect[i++] = *iter;

    std::sort(vect.begin(), vect.end(), comparer<std::string, unsigned int>);
    vect.resize(count);
    return vect;
}

bool MonogramCollector::dump(char const* filename)
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
