#include <cassert> // assert
#include <iostream> // cout
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

    // ѕытаемс€ получить заголовок документа. ƒаже если не получили всЄ равно
    // возвращаем true потому что update родител€ шагом выше отработало.
    char const* const title = elem->Attribute("title");
    if(!title)
        return true;

    std::string const text = title;

    unsigned int hash = 0;
    std::size_t beg = 0;
    std::size_t end = 0;

    while((hash = getTokenID(text, beg, end)) != 0)
    {
        // -1 в младшем полуслове означает что эта зона заголовка документа. -1
        // имеет суффикс "ul" потому что нам нужны младшие 32 бита. ƒалее это
        // значение рассматриваетс€ как зона заголовока документа.
        std::vector<unsigned long long>& vect = _tokenToPos[hash];
        vect.push_back(((unsigned long long)docID << 32) | -1ul);
        ++_totalTokenCount;
        beg = end;
    }

    return false;
}

bool ZoneIndexMaker::write(std::string const& filename)
{
    std::cout << "–азбиваю документ на зоны..." << std::endl;

    // “рансформируем координаты в зоны.
    for(std::map<unsigned int, std::vector<unsigned long long>>::iterator iter = _tokenToPos.begin(), end = _tokenToPos.end(); iter != end; ++iter)
    {
        std::vector<unsigned long long>& vect = iter->second;
        std::vector<unsigned long long> zones;

        assert(!vect.empty());

        for(std::size_t i = 0; i < vect.size(); ++i)
        {
            unsigned long long index = vect[i];

            unsigned int const docID = index >> 32;
            int const pos = index & -1l;

            // »дентификатор документа всегда ненулевой.
            assert(docID != 0);

            unsigned int currentZone = 0;

            // Ётот список можно дополн€ть если вдруг станет необходимым
            // добавить ещЄ какой-нибудь тип зон.
            if(pos == -1)
            {
                currentZone = 1;
            }
            else if(pos >= 0)
            {
                // —ловопозици€ не может быть больше количества слов в
                // документе. +2 добавление к зоне потому что 0 зарезервирован,
                // а 1 зан€т заголовком.
                assert((unsigned int)pos <= _docIDToTokenCount[docID]);
                currentZone = pos / (_docIDToTokenCount[docID] / _numberOfZones) + 2;
            }

            // ¬ текущей версии индекс зоны не может быть равен нулю.
            assert(currentZone > 0);

            index = ((unsigned long long)docID << 32) | currentZone;

            // —писок сортированный. ƒостаточно проверить последний индекс.
            if(zones.empty() || zones.back() != index)
                zones.push_back(index);
        }

        assert(!zones.empty());
        assert(zones.size() <= vect.size());

        // ћен€ем информаци€ о словопозици€х на информацию о зонах.
        iter->second = zones;
    }

    return PositionalIndexMaker::write(filename);
}
