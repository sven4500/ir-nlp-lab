#include <cassert>
#include "bigramcollector.h"
#include "textbeautifier.h"
using namespace tinyxml2;

BigramCollector::BigramCollector()
{}

BigramCollector::~BigramCollector()
{}

void BigramCollector::rememberMostFrequent(std::vector<std::pair<std::string, unsigned int>> const& frequent)
{
    _mostFrequent = std::map<std::string, unsigned int>(frequent.begin(), frequent.end());
}

void BigramCollector::rememberMostFrequent(std::map<std::string, unsigned int> const& frequent)
{
    _mostFrequent = frequent;
}

std::string BigramCollector::first(unsigned int const i)const
{
    std::string const& str = operator[](i);
    std::size_t const pos = str.find(' ');
    return str.substr(0, pos);
}

std::string BigramCollector::second(unsigned int const i)const
{
    std::string const& str = operator[](i);
    std::size_t const pos = str.find(' ');
    return str.substr(pos, std::string::npos);
}

void BigramCollector::update(XMLElement const* elem)
{
    // ��������� �������� ��� ������� UTF-8.
    std::string text = elem->GetText();
    wipe_all(text);

    std::size_t pos = 0;
    std::string token;
    std::string lastToken;

    while((pos = extract_token(text, token, pos)) != std::string::npos)
    {
        // extract_token ����������� ��� ������ �� ������ ������� ���
        // ������������ ���� �� ���� ������ � �������� ����� ���������� �����.
        if(!is_alpha_numeric(token[0]))
        {
            to_lower_case(token);

            // ������� ����� ������ ���������� ������ ���� �� �� �������� �����
            // ������������� � �� �������� ������ ��� ���������� ������.
            if(_mostFrequent.find(token) == _mostFrequent.cend())
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
