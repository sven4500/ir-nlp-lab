#include <fstream>
#include <iostream>
#include "invertedIndexMaker.h"
#include "crc32.h"
using namespace tinyxml2;

InvertedIndexMaker::InvertedIndexMaker(): _meanChars(0)
{}

InvertedIndexMaker::~InvertedIndexMaker()
{}

void InvertedIndexMaker::clear()
{
    _termToDocID.clear();
    _meanChars = 0;
}

std::size_t InvertedIndexMaker::termCount()const
{
    return _termToDocID.size();
}

std::size_t InvertedIndexMaker::meanChars()const
{
    return _meanChars;
}

bool InvertedIndexMaker::update(XMLElement const* elem)
{
    unsigned int const docID = elem->IntAttribute("id", 0);
	if(docID == 0)
		return false;

	std::size_t pos = 0;
	std::size_t end = 0;

	std::string const text = elem->GetText();

	// tinyxml2 ������������� ������������ ���� CR+LF � LF, �������
	// ��� ����������� ���� ������ LF.
	while((end = text.find_first_of('\n', pos)) != std::string::npos)
	{
		std::size_t const size = end - pos;
		if(size > 1)
		{
			_meanChars = (_meanChars + size) / 2;
			std::string const token = text.substr(pos, size);
			unsigned int const termID = crc32(0, token.c_str(), token.length());
			_termToDocID[termID].push_back(docID);
		}
		pos = end + 1;
	}

	return !text.empty();
}

bool InvertedIndexMaker::write(std::string const& filename)
{
    std::ofstream fout;
	fout.open(filename, std::ios::binary | std::ios::out | std::ios::trunc);
	if(!fout)
		return false;

	// ���������� ���� ��������.
	std::size_t const termCount = _termToDocID.size();

	// �������: �������������� ������ � ��������� ������ ������� � ������.
	std::vector<unsigned int> lookupTable(2 * termCount, 0);
	unsigned int const lookupTableBytes = lookupTable.size() * sizeof(int);
	
	// ���������� ��������� �����.
	unsigned int const fileHeader[4] = {0xABABABAB, sizeof(fileHeader) + lookupTableBytes};
	fout.write((char*)fileHeader, sizeof(fileHeader));

	// ���� ���������� �������. ������� ��� �����.
	fout.seekp(lookupTableBytes, std::ios::cur);

	std::map<unsigned int, std::vector<unsigned int> >::const_iterator iter = _termToDocID.cbegin();
	//std::map<unsigned int, std::vector<unsigned int> >::const_iterator const end = termToDocID.cend();

	std::cout << "��������� ����..." << std::endl;

	for(std::size_t i = 0; i < termCount; ++i)
	{
		unsigned int const& termID = iter->first;
		std::vector<unsigned int> const& docID = iter->second;

		// ��������� ���������� � �������� ������ � �������.
		lookupTable[2*i+0] = termID;
		lookupTable[2*i+1] = (unsigned int)fout.tellp();

		// ��������� �������. ������� ������ ������������� ���� ���������.
		unsigned int const termHeader[4] = {0xAAAAAAAA, termID, docID.size()};

		// ���������� ���������� � �������.
		fout.write((char*)termHeader, sizeof(termHeader));
		if(!docID.empty())
			fout.write((char*)&docID[0], docID.size() * sizeof(int));

		++iter;

		if(i % 10000 == 0)
			std::cout << "\r�������� ��������: " << i << '/' << termCount;
	}

	std::cout << "\n��������� �������..." << std::endl;

	// ���������� ������� � ����.
	fout.seekp(sizeof(fileHeader), std::ios::beg);
	fout.write((char*)&lookupTable[0], lookupTableBytes);

	fout.close();
	std::cout << "���� ������� �������." << std::endl;
	return true;
}
