#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <tinyxml2.h>
#include <crc32.h>
using namespace tinyxml2;

bool updateIndex(std::map<unsigned int, std::vector<unsigned int>>& termToDocID, XMLElement const* elem)
{
	unsigned int const docID = elem->IntAttribute("id", 0);
	if(docID == 0)
		return false;

	std::size_t pos = 0;
	std::size_t end = 0;

	std::string const text = elem->GetText();

	// tinyxml2 автоматически конвертирует пару CR+LF в LF, поэтому
	// как разделитель ищем только LF.
	while((end = text.find_first_of('\n', pos)) != std::string::npos)
	{
		std::size_t const size = end - pos;
		if(size > 1)
		{
			std::string const token = text.substr(pos, size);
			unsigned int const termID = crc32(0, token.c_str(), token.length());
			termToDocID[termID].push_back(docID);
		}
		pos = end + 1;
	}

	return !text.empty();
}

bool writeIndex(std::map<unsigned int, std::vector<unsigned int>>& termToDocID, std::string const& fileName)
{
	std::ofstream fout;
	fout.open(fileName, std::ios::binary | std::ios::out | std::ios::trunc);
	if(!fout)
		return false;

	// Записываем заголовок файла.
	unsigned int const header[4] = {0xABABABAB};
	fout.write((char*)&header, sizeof(header));

	std::map<unsigned int, std::vector<unsigned int> >::const_iterator iter = termToDocID.cbegin();
	std::map<unsigned int, std::vector<unsigned int> >::const_iterator const end = termToDocID.cend();

	while(iter != end)
	{
		unsigned int const termID[2] = {0xAAAAAAAA, iter->first};
		std::vector<unsigned int> const& docID = iter->second;
		
		fout.write((char*)termID, sizeof(termID));
		if(!docID.empty())
			fout.write((char*)&docID[0], docID.size() * sizeof(int));

		++iter;
	}

	fout.flush();
	fout.close();

	return true;
}

int main(int argc, char** argv)
{
	setlocale(LC_CTYPE, "Russian");

	if(argc != 3)
	{
		std::cout << "IR3.exe _In_tokens.xml _Out_index.dat" << std::endl;
		return 1;
	}

	XMLError err = XML_SUCCESS;

	XMLDocument doc;
	err = doc.LoadFile(argv[1]);
	if(err != XML_SUCCESS)
	{
		std::cout << "Не удалось открыть XML файл" << std::cout;
		return 1;
	}

	std::cout << "Обрабатываю файл: " << argv[1] << std::endl;

	std::map<unsigned int, std::vector<unsigned int>> index;
	unsigned int pages = 0;

	XMLElement const* root = doc.FirstChildElement();
	XMLElement const* pageElem = (root) ? root->FirstChildElement() : 0;

	while(pageElem != 0)
	{
		updateIndex(index, pageElem);

		if(pages % 500 == 0)
			std::cout << "\rСтатей обработано: " << pages;
		//if(pages == 2000)
			//break;
		++pages;

		pageElem = pageElem->NextSiblingElement();
	}

	writeIndex(index, argv[2]);
	return 0;
}
