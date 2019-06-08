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
        // �������� ������ � ��������� �����.
        // � ����� ������� � �����
        //s.insert(tokenID)
    }
    ++_docCount;
    return false;
}

bool TFIDFMaker::write(std::string const& fn)
{
    return false;
}
