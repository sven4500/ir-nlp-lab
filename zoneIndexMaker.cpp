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
        // -1 в младшем получлове означает что эта зона заголовка документа. -1
        // имеет одиночный суффикс "l" потому что нам нужны младшие 32 бита.
        _tokenToPos[hash].push_back(((unsigned long long)docID << 32) | (-1l));
        ++_totalTokenCount;
        beg = end;
    }

    return false;
}

bool ZoneIndexMaker::write(std::string const& filename)
{
    std::cout << "Разбиваю документ на зоны..." << std::endl;

    // Трансформируем координаты в зоны.
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

            // Идентификатор документа всегда ненулевой. Словопозиция не должна
            // превышать количество слов в документе.
            assert(docID != 0);
            //assert(pos <= _docIDToTokenCount[docID]);

            unsigned int currentZone = 0;

            // Этот список можно дополнять если вдруг станет необходимым
            // добавить ещё какой-нибудь тип зон.
            if(pos == -1)
                currentZone = 1;
            else if(pos >= 0)
                currentZone = pos / (_docIDToTokenCount[docID] / _numberOfZones) + 2;

            // В текущей версии индекс зоны не может быть равен нулю.
            assert(currentZone > 0);

            index = ((unsigned long long)docID << 32) | currentZone;

            // Список сортированный. Достаточно проверить последний индекс.
            if(zones.empty() || zones.back() != index)
                zones.push_back(index);
        }

        assert(!zones.empty());
        assert(zones.size() <= vect.size());

        // Меняем информация о словопозициях на информацию о зонах.
        iter->second = zones;
    }

    return PositionalIndexMaker::write(filename);
}
