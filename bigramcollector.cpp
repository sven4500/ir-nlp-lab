#include <cassert>
#include "bigramcollector.h"
#include "textbeautifier.h"
using namespace tinyxml2;

template class BigramCollector<unsigned int>;
template class BigramCollector<double>;

template<typename T>
BigramCollector<T>::BigramCollector()
{}

template<typename T>
BigramCollector<T>::~BigramCollector()
{}

template<typename T>
void BigramCollector<T>::rememberMostFrequent(std::vector<std::pair<std::string, T>> const& frequent)
{
    _mostFrequent = std::map<std::string, T>(frequent.cbegin(), frequent.cend());
}

template<typename T>
void BigramCollector<T>::rememberMostFrequent(std::map<std::string, T> const& frequent)
{
    _mostFrequent = frequent;
}

template<typename T>
void BigramCollector<T>::update(XMLElement const* elem)
{
    // Исключаем ненужные нам символы UTF-8.
    std::string text = elem->GetText();
    wipe_all(text);

    std::size_t pos = 0;
    std::string token;
    std::string lastToken;

    while((pos = extract_token(text, token, pos)) != std::string::npos)
    {
        // extract_token гарантирует что строка не пустая поэтому там
        // присутствует хотя бы один символ к которому можем обращаться сразу.
        if(!is_alpha_numeric(token[0]))
        {
            to_lower_case(token);

            // Считаем токен частью коллокации только если он не является часто
            // употребляемым и не является цифрой или английским словом.
            if(_mostFrequent.find(token) == _mostFrequent.cend())
            {
                if(!lastToken.empty())
                {
                    _collection[lastToken + " " + token]++;
                    ++_totalCount;
                }

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
