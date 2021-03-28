#include <iostream>
#include <algorithm> // std::sort, sdt::reverse
#include <string>
#include <fstream>
#include <clocale>
#include <map>
#include <vector>
#include <crc32.h>
#include <tinyxml2.h>
#include <unicode\unistr.h>
using namespace tinyxml2;

void processPage(XMLElement const* const elem, std::map<unsigned int, unsigned int>& map, bool russianOnly = false)
{
	std::size_t pos = 0;
	std::size_t end = 0;

	std::string const text = elem->GetText();

	while((end = text.find_first_of('\n', pos)) != std::string::npos)
	{
		std::size_t const size = end - pos;
		if(size >= 1)
		{
			bool process = true;
			std::string const token = text.substr(pos, size);

			if(russianOnly)
			{
				// ���� ������ ������������ ������ ������ �� ������� �����.
				icu::UnicodeString uniToken(token.c_str(), "UTF8");
				UChar ch = uniToken.charAt(0);
				if(false)
					process = false;
			}

			if(process)
			{
				unsigned int const termID = crc32(0, token.c_str(), token.length());
				map[termID]++;
			}
		}
		pos = end + 1;
	}
}

bool writeOutput(std::string const& filename, std::map<unsigned int, unsigned int> const& map)
{
	std::vector<unsigned int> vect;
	for(std::map<unsigned int, unsigned int>::const_iterator iter = map.cbegin(); iter != map.cend(); ++iter)
		vect.push_back(iter->second);
	std::sort(vect.begin(), vect.end());
	std::reverse(vect.begin(), vect.end());
	
	std::ofstream fout;
	fout.open(filename, std::ios::out | std::ios::trunc);
	if(!fout)
		return false;

	for(std::size_t i = 0; i < vect.size(); ++i)
		fout << vect[i] << ' ';
	fout.flush();
	fout.close();
	return true;
}

int main(int argc, char** argv)
{
	std::setlocale(LC_CTYPE, "Russian");

	if(argc != 3)
	{
		std::cout << "NLP2.exe ������.xml ����.txt" << std::endl;
		return -1;
	}

	// � ���� ����� ����� ������� ����������� ��� ��� ������
	// (�� ���� ��� �����) �� ��� �������.
	std::map<unsigned int, unsigned int> tokenIDToFreq;

	XMLError err = XML_SUCCESS;

	XMLDocument doc;
	err = doc.LoadFile(argv[1]);
	if(err != XML_SUCCESS)
	{
		std::cout << "�� ������� ������� XML ����." << std::endl;
		return -1;
	}

	unsigned int numPagesProcessed = 0;

	XMLElement* const root = doc.FirstChildElement();
	XMLElement* pageElem = (root) ? root->FirstChildElement() : 0;
	while(pageElem)
	{
		processPage(pageElem, tokenIDToFreq);
		++numPagesProcessed;
		if(numPagesProcessed % 500 == 0)
			std::cout << "\r���������� ������: " << numPagesProcessed;
		#ifdef _DEBUG
		if(numPagesProcessed > 2000)
			break;
		#endif
		pageElem = pageElem->NextSiblingElement();
	}

	std::cout << "\n������� ��������� �������. ���������� �������: " << tokenIDToFreq.size() << ".\n"
		<< "��������� ������ � ����..." << std::endl;

	bool success = false;
	success = writeOutput(argv[2], tokenIDToFreq);
	if(success == false)
	{
		std::cout << "�� ������� �������� ������ � ����." << std::endl;
		return -1;
	}
	std::cout << "���� ������� �������." << std::endl;

	return 0;
}
