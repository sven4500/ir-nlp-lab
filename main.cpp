#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <tinyxml2.h>
#include <crc32.h>
using namespace tinyxml2;

bool updateIndex(std::map<unsigned int, std::vector<unsigned int>>& index, XMLElement const* elem)
{
	unsigned int const id = elem->IntAttribute("id", 0);
	if(id == 0)
		return false;

	std::size_t pos = 0;
	std::size_t end = 0;

	std::string const text = elem->GetText();

	while((end = text.find_first_of('\n', pos)) != std::string::npos)
	{
		std::size_t const size = end - pos;
		if(size > 1)
		{
			std::string const token = text.substr(pos, size);
			unsigned int const hash = crc32(0, token.c_str(), token.length());
			index[hash].push_back(id);
		}
		pos = end + 1;
	}

	return !text.empty();
}

bool writeIndex(std::map<unsigned int, std::vector<unsigned int>>& index, std::string const& fileName)
{
	return true;
}

int main(int argc, char** argv)
{
	setlocale(LC_CTYPE, "Russian");

	if(argc != 3)
	{
		std::cout << "IR3.exe tokens.xml index.dat" << std::endl;
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

		if(pages % 1000 == 0)
			std::cout << "\rСтатей обработано: " << pages;
		++pages;
		//if(pages == 1000) break;

		pageElem = pageElem->NextSiblingElement();
	}

	writeIndex(index, argv[2]);
	return 0;
}
