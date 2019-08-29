#include "ZoneIndexMaker.h"
using namespace tinyxml2;

ZoneIndexMaker::ZoneIndexMaker()
{}

ZoneIndexMaker::~ZoneIndexMaker()
{}

void ZoneIndexMaker::clear()
{}

bool ZoneIndexMaker::update(XMLElement const* const elem)
{
    PositionalIndexMaker::update(elem);
    // TODO: добавить токены из заголовка документа.
    return false;
}

bool ZoneIndexMaker::write(std::string const& filename)
{
    // TODO: трансформировать координаты в зоны.
    return PositionalIndexMaker::write(filename);
}
