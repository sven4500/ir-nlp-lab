#include <set>
#include "TFIDFMaker.h"
using namespace tinyxml2;

TFIDFMaker::TFIDFMaker(): _docCount(0)
{}

TFIDFMaker::~TFIDFMaker()
{}

void TFIDFMaker::clear()
{
    _docCount = 0;
    _tok2DocFreq.clear();
    _tok2CorpFreq.clear();
}

bool TFIDFMaker::update(XMLElement const* elem)
{
    //std::set<unsigned int> s;
    //while(true)
    {
        // Помещаем токены в локальный набор.
        // В конце добавим в карту
        //s.insert(tokenID)
    }
    ++_docCount;
    return false;
}

bool TFIDFMaker::write(std::string const& fn)
{
    return false;
}
