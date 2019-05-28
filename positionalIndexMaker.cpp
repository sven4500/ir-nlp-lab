#include <unicode\unistr.h>
#include <algorithm>
#include <crc32.h>
#include "positionalIndexMaker.h"
using namespace tinyxml2;

PositionalIndexMaker::PositionalIndexMaker()
{}

PositionalIndexMaker::~PositionalIndexMaker()
{}

void PositionalIndexMaker::clear()
{
    _tokenToPos.clear();
}

bool PositionalIndexMaker::update(XMLElement const* const elem)
{
    unsigned int const docID = elem->IntAttribute("id", 0);
    if(docID == 0)
        return false;

    std::string const text = elem->GetText();

    static std::string const delim(" ,.!?@$#&%_/*-+|<>(){}[]:;=`\\\"\'\x0A\x0D");
    /*for(std::size_t i = 0; i < delim.size(); ++i)
        std::replace(text.begin(), text.end(), delim[i], ' ');*/

    std::size_t pos = 0;
    std::size_t beg = 0;
    std::size_t end = 0;

    while((beg = text.find_first_not_of(delim, beg)) != std::string::npos && (end = text.find_first_of(delim, beg)) != std::string::npos)
    {
        std::string token = text.substr(beg, end - beg);

        // Средствами библиотеки ICU преобразуем строку в нижний регистр.
        {
            icu::UnicodeString uniToken(token.c_str(), "UTF8");
            uniToken.toLower();
            token.clear();
            uniToken.toUTF8String(token);
        }

        unsigned int const hash = crc32(0, token.c_str(), token.size());
        _tokenToPos[hash].push_back(((unsigned long long)docID << 32) | pos);

        beg = end;
        ++pos;
    }

    return true;
}

bool PositionalIndexMaker::write(std::string const& filename)
{
    return false;
}
