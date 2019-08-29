#include "ZoneIndexMaker.h"
using namespace tinyxml2;

ZoneIndexMaker::ZoneIndexMaker()
{}

ZoneIndexMaker::~ZoneIndexMaker()
{}

bool ZoneIndexMaker::update(XMLElement const* const elem)
{
    unsigned int const docID = elem->IntAttribute("id", 0);
    if(docID == 0)
        return false;

    bool const success = PositionalIndexMaker::update(elem);
    if(!success)
        return false;

    // Пытаемся получить заголовок документа. Даже если не получили всё равно
    // возвращаем true потому что update родителя шагом выше отработало.
    char const* const title = elem->Attribute("title");
    if(!title)
        return true;

    std::string const text = title;

    unsigned int hash = 0;
    std::size_t beg = 0;
    std::size_t end = 0;

    while((hash = getTokenID(text, beg, end)) != 0)
    {
        // -1 в младшем получлове означает что эта зона заголовка документа.
        _tokenToPos[hash].push_back(((unsigned long long)docID << 32) | (-1l));
        ++_tokenCount;
        beg = end;
    }


    return false;
}

bool ZoneIndexMaker::write(std::string const& filename)
{
    // Трансформируем координаты в зоны.
    for(std::map<unsigned int, std::vector<unsigned long long>>::iterator iter = _tokenToPos.begin(), end = _tokenToPos.end(); iter != end; ++iter)
    {
        int posMin = std::numeric_limits<int>::max(), posMax = std::numeric_limits<int>::min();

        std::vector<unsigned long long>& vect = iter->second;
        /*for(std::size_t i = 0; i < vect.size(); ++i)
        {
            if(vect[i] & 0xFFFFFFFF)
            {}
        }*/
    }

    return PositionalIndexMaker::write(filename);
}
