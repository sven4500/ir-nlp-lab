#include <unicode\unistr.h>
#include <set>
#include <fstream>
#include <iostream>
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
    _tokFreq.clear();
}

unsigned int TFIDFMaker::docCount()const
{
    return _docCount;
}

unsigned int TFIDFMaker::tokenCount()const
{
    return _tokFreq.size();
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
    unsigned int tokTotalCount = 0;

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
        ++tokTotalCount;

        beg = end;
    }

    // ������ �� ����� ���������� ���� � ��������� � ��������� ��� ����������.
    // ����������� ������� ��� ���������� � ����� ����� ����������.
    for(std::map<unsigned int, DocFreq>::iterator iter = tok2DocFreq.begin(), end = tok2DocFreq.end(); iter != end; ++iter)
    {
        iter->second._docID = docID;
        iter->second._tokenTotalCount = tokTotalCount;

        // ��������� ������ �� ��������� � ������� �� ��������� ������������.
        if(iter->second._tokenCount < iter->second._tokenTotalCount)
            _tokFreq[iter->first].push_back(iter->second);
    }

    ++_docCount;
    return true;
}

bool TFIDFMaker::write(std::string const& fn)
{
    std::ofstream fout;
    fout.open(fn, std::ios::out | std::ios::trunc | std::ios::binary);
    if(!fout)
    {
        std::cout << "�� ���� ������� ���� \"" << fn << "\" ��� ������." << std::endl;
        return false;
    }

    // ���������� ��������� �����.
    struct
    {
        unsigned int _sign;
        unsigned int _tokCount;
        unsigned int _docCount;
        unsigned int _unused;
    }fileHd = {0x8C8C8C8C, tokenCount(), docCount(), 0};

    fout.write((char*)&fileHd, sizeof(fileHd));

    struct DictItem
    {
        unsigned int _tokenID;
        unsigned int _docCount;     // � �������� ���������� �����������
        unsigned int _offBytes;     // ��� ������ ������ � �����
        unsigned int _unused;
    };

    std::vector<DictItem> dict(tokenCount());
    fout.seekp(sizeof(DictItem) * tokenCount(), std::ios::cur);

    std::map<unsigned int, std::vector<DocFreq>>::iterator iter = _tokFreq.begin();

    for(unsigned int i = 0, iMax = tokenCount(); i < iMax; ++i)
    {
        dict[i]._tokenID = iter->first;
        dict[i]._docCount = iter->second.size();
        dict[i]._offBytes = (unsigned int)fout.tellp();
        dict[i]._unused = 0;

        fout.write((char*)&iter->second[0], sizeof(DocFreq) * iter->second.size());

        if(i % 5000 == 0)
            std::cout << '\r' << (i * 100) / iMax << '%';

        ++iter;
    }

    std::cout << std::endl;
    std::cout << "��������� �������." << std::endl;

    fout.seekp(sizeof(fileHd), std::ios::beg);
    fout.write((char*)&dict[0], sizeof(DictItem) * tokenCount());

    fout.close();
    std::cout << "���� ������� �������." << std::endl;
    return false;
}
