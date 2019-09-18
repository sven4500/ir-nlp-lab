#include <cassert>
#include "bigramcollector.h"
#include "textbeautifier.h"
using namespace tinyxml2;

BigramCollector::BigramCollector()
{}

BigramCollector::~BigramCollector()
{}

void BigramCollector::tellMostFrequent(std::vector<std::pair<std::string, unsigned int>> const& frequent)
{
    _mostFrequent = std::map<std::string, unsigned int>(frequent.begin(), frequent.end());
}

void BigramCollector::tellMostFrequent(std::map<std::string, unsigned int> const& frequent)
{
    _mostFrequent = frequent;
}

void BigramCollector::update(XMLElement const* elem)
{
    // Исключаем ненужные нам символы UTF-8.
    std::string text = elem->GetText();
    wipe_all(text);

    std::size_t pos = 0;
    std::string token;
    std::string lastToken;

    while((pos = extract_token(text, token, pos)) != std::string::npos)
    {
        assert(!token.empty());
        if(!is_alpha_numeric(token[0]))
        {
            to_lower_case(token);
            // Считаем токен частью коллокации только если он не является часто
            // употребляемым и не является цифрой или английским словом.
            if(_mostFrequent.find(token) == _mostFrequent.end())
            {
                if(!lastToken.empty())
                    _collection[lastToken + " " + token]++;
                lastToken = token;
            }
            else
            {
                lastToken.clear();
            }
        }
        else
        {
            lastToken.clear();
        }
    }
}
