#include <algorithm>
#include <fstream>
#include <iostream>
#include "normIndexMaker.h"
#include "crc32.h"
using namespace std;

size_t NormIndexMaker::equalSize(string const& s1, string const& s2)
{
    size_t const size = (min(s1.size(), s2.size()) / 2) * 2;
    if(size < 2)
        return 0;

    size_t i = 0;
    for(i = 0; i < size; i += 2)
        if(*(short*)&s1[i] != *(short*)&s2[i])
            break;

    return i;
}

NormIndexMaker::NormIndexMaker()
{

}

NormIndexMaker::~NormIndexMaker()
{

}

void NormIndexMaker::addAsTerm(string const& token, unsigned int const docID)
{
    // ����� ����� ��������� �������� end ������ ��� ���� ������ �����������
    // ����� ����������� �����.
    map<string, vector<unsigned int>>::iterator begin = _termToDocID.begin(), end = _termToDocID.end(), iter = begin;

    while(true)
    {
        if(iter == end)
        {
            // ���� �������� �����, �� ��������� ������ � ������ �� ��������.
            _termToDocID[token].push_back(docID);
            break;
        }
        else if(iter->first == token)
        {
            // ���� ��� ����� ����� ������ �� ��������� ������ �� ��������.
            iter->second.push_back(docID);
            break;
        }
        else if(token < iter->first)
        {
            // ����� �� ���� ������� ����� ������� ������� ������ ������. ���
            // ����� �������� ��� ���� (1) ��� ������������ ����
            // (2) ������������ � ���������� ���������.

            // ����� ���������� ���� ������� ����� �����������������.
            static size_t const threshold = 6;
            size_t eqSize = 0;

            // �������� ������ ����������� ����� � ��������� ����� ������.
            // ���� � �������� �����������, �� ������� ������������� �������.
            if((eqSize = equalSize(token, iter->first)) && (iter->first.size() - eqSize < threshold))
            {
                std::string const term = token.substr(0, eqSize);
                iter->second.push_back(docID);
                _termToDocID[term] = iter->second;
                _termToDocID.erase(iter);
            }
            else if((iter != begin) && (&(--iter)) && (eqSize = equalSize(token, iter->first)) && (iter->first.size() - eqSize < threshold))
            {
                // ������� ������! ������������������ �������� ��� ��� �����
                // ����� ��������� ���� ����� ��������.
                //iter->second.push_back(docID);
                std::string const term = token.substr(0, eqSize);
                iter->second.push_back(docID);
                _termToDocID[term] = iter->second;
                _termToDocID.erase(iter);
            }
            else
            {
                _termToDocID[token].push_back(docID);
            }

            break;
        }

        ++iter;
    }
}

bool NormIndexMaker::update(tinyxml2::XMLElement const* const elem)
{
    unsigned int const docID = elem->IntAttribute("id", 0);
	if(docID == 0)
		return false;

	std::size_t pos = 0;
	std::size_t end = 0;

	std::string const text = elem->GetText();

	// tinyxml2 ������������� ������������ ���� CR+LF � LF, ������� ���
    // ����������� ���� ������ LF.
	while((end = text.find_first_of('\n', pos)) != std::string::npos)
	{
		std::size_t const size = end - pos;
        if(size > 0)
        {
            std::string const token = text.substr(pos, size);
            addAsTerm(token, docID);
        }
		pos = end + 1;
	}

	return !_termToDocID.empty();
}

bool NormIndexMaker::write(std::string const& filename)
{
    std::cout << std::endl << "����� �������� ���������������� "
        << _termToDocID.size() << std::endl;

    {
        std::ofstream fout;
        fout.open("normtokens.txt", std::ios::trunc | std::ios::out);
        for(auto iter = _termToDocID.begin(), end = _termToDocID.end(); iter != end; ++iter)
            fout << iter->first << std::endl;
        fout.close();
    }

    return true;

    for(map<string, vector<unsigned int>>::iterator iter = _termToDocID.begin(), end = _termToDocID.end(); iter != end; ++iter)
    {
        unsigned int const termID = crc32(0, iter->first.c_str(), iter->first.size());
        InvertedIndexMaker::_termToDocID[termID] = iter->second;
    }

    // �������� ������������ ���������� ������ write.
    return InvertedIndexMaker::write(filename);
}
