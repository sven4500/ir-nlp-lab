#include <iostream>
#include <fstream>
#include <string>
#include <tinyxml2.h>
using namespace tinyxml2;

int main(int argc, char** argv)
{
	static unsigned int const ID_SRCE = 1;
	static unsigned int const ID_DEST = 2;
	static unsigned int const NUM_ARGS = 3;

	// Для русских символов (можно обойтись LC_CTYPE).
	setlocale(LC_ALL, "Russian");

	if(argc < NUM_ARGS)
	{
		std::cout << "IR1.exe [входной XML файл] [выходной каталог]" << std::endl;
		return 1;
	}

	XMLError err = XML_SUCCESS;

	XMLDocument doc;
	err = doc.LoadFile(argv[ID_SRCE]);

	if(err != XML_SUCCESS)
	{
		std::cout << "Не удалось открыть XML документ. Код ошибки (" << err << ")." << std::endl;
		return 1;
	}

	std::cout << "Обрабатываю файл: " << argv[ID_SRCE] << std::endl;

	// Достаём корневой элемент (mediawiki).
	XMLElement* const root = doc.RootElement();
	if(root == 0)
	{
		std::cout << "Не найден корневой элемент." << std::endl;
		return 1;
	}

	unsigned int pages = 0;

	XMLElement* pageElem = root->FirstChildElement("page");
	while(pageElem != 0)
	{
		XMLElement* revisionElem = pageElem->FirstChildElement("revision");
		if(revisionElem != 0)
		{
			XMLElement* idElem = revisionElem->FirstChildElement("id");
			XMLElement* timestampElem = revisionElem->FirstChildElement("timestamp");
			XMLElement* textElem = revisionElem->FirstChildElement("text");

			if(idElem && timestampElem && textElem)
			{
				//XMLNode* textChildElem = textElem->FirstChild();
				//XMLText* text = textChildElem->ToText();
		
				std::string const filename = std::string(argv[ID_DEST]) + "/" + idElem->GetText() + "_" + std::string(timestampElem->GetText()).substr(0, 10) + ".txt";
				std::ofstream fout(filename, std::ios::out);
				if(fout)
				{
					fout << textElem->GetText();
					fout.close();
				}
				else
				{
					std::cout << "Не удалось создать файл." << std::endl;
				}

				++pages;
				if(pages % 1000 == 0)
					std::cout << "\r\rСтатей обработано: " << pages;
			}
		}

		pageElem = pageElem->NextSiblingElement("page");
	}

	return 0;
}
