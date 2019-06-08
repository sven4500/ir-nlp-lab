#include <unicode\unistr.h>
#include <set>
#include <string>
#include <crc32.h>
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
}

bool TFIDFMaker::update(XMLElement const* const elem)
{
    unsigned int const docID = elem->IntAttribute("id", 0);
    if(docID == 0)
        return false;

    // ������� ����������� �������.
    static char const* const delim = " ,.!?@$#&%_/*-+|<>(){}[]:;=`\\\"\'\x0A\x0D";

    std::map<unsigned int, DocFreq> tok2DocFreq;

    // ����� ���������� ������� � ���������.
    unsigned int tokenTotalCount = 0;

    std::string const text = elem->GetText();

    std::size_t beg = 0;
    std::size_t end = 0;

    while((beg = text.find_first_not_of(delim, beg)) != std::string::npos && (end = text.find_first_of(delim, beg)) != std::string::npos)
    {
        std::string token = text.substr(beg, end - beg);

        // ���������� ���������� ICU ����������� ������� � ������ �������.
        {
            icu::UnicodeString uniToken(token.c_str(), "UTF8");
            uniToken.toLower();
            token.clear();
            uniToken.toUTF8String(token);
        }

        // ������� ����������� ����� ������.
        unsigned int const hash = crc32(0, token.c_str(), token.size());

        // ����������� ���������� ��������� �������� ������ � �������� � �����
        // ���������� ������� � ���������.
        ++tok2DocFreq[hash]._tokenCount;
        ++tokenTotalCount;

        beg = end;
    }

    // ������ �� ����� ���������� ���� � ��������� � ��������� ��� ����������.
    // ����������� ������� ��� ���������� � ����� ����� ����������.
    for(std::map<unsigned int, DocFreq>::iterator iter = tok2DocFreq.begin(), end = tok2DocFreq.end(); iter != end; ++iter)
    {
        iter->second._docID = docID;
        iter->second._tokenTotalCount = tokenTotalCount;

        // ��������� ������ �� ��������� � ������� �� ��������� ������������.
        if(iter->second._tokenCount < iter->second._tokenTotalCount)
            _tok2DocFreq[iter->first].push_back(iter->second);
    }

    ++_docCount;
    return true;
}

bool TFIDFMaker::write(std::string const& fn)
{
    return false;
}
