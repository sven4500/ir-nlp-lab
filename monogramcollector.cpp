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

unsigned int MonogramCollector::tokenCount()const
{
    return _tokens.size();
}

// https://stackoverflow.com/questions/19842035/how-can-i-sort-a-stdmap-first-by-value-then-by-key
template<typename ty1, typename ty2>
bool MonogramCollector::comparer(std::pair<ty1, ty2> const& a, std::pair<ty1, ty2> const& b)
{
    return (a.second != b.second) ? a.second > b.second : false; //a.first < b.first;
}

void MonogramCollector::update(XMLElement const* elem)
{
    // Исключаем ненужные нам символы UTF-8.
    std::string text = elem->GetText();
    wipe_all(text);

    std::size_t pos = 0;
    std::string token;

    while((pos = extract_token(text, token, pos)) != std::string::npos)
    {
        if(!token.empty() && !is_alpha_numeric(token[0]))
        {
            to_lower_case(token);
            _tokens[token]++;
        }
    }
}

bool MonogramCollector::dump(char const* filename)
{
    std::vector<std::pair<std::string, unsigned int>> vect;
    vect.resize(_tokens.size());
    unsigned int i = 0;
    for(auto iter = _tokens.begin(), end = _tokens.end(); iter != end; ++iter)
        vect[i++] = *iter;
    std::sort(vect.begin(), vect.end(), comparer<std::string, unsigned int>);
    std::ofstream fout;
    fout.open(filename, std::ios::out | std::ios::trunc);
    for(std::size_t i = 0; i < 100; ++i)
        fout << vect[i].second << ": " << vect[i].first << "\n";
    fout.close();
    return false;
}
