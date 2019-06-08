#include <iostream>
#include <tinyxml2.h>
using namespace tinyxml2;

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");

	if(argc != 3)
	{
		std::cout << "IR1.exe [входной XML файл] [выходной файл]" << std::endl;
		return 1;
	}

	XMLError err = XML_SUCCESS;

	XMLDocument doc1;
	err = doc1.LoadFile(argv[1]);
	if(err != XML_SUCCESS)
	{
		std::cout << "Не удалось открыть XML документ. Код ошибки (" << err << ")." << std::endl;
		return 1;
	}

	XMLElement const* const root1 = doc1.FirstChildElement();
	if(!root1)
	{
		std::cout << "Не найден корневой элемент." << std::endl;
		return 1;
	}

	XMLDocument doc2;
	err = doc2.LoadFile(argv[2]);

	XMLElement* root2 = doc2.FirstChildElement();
	if(!root2)
	{
		root2 = doc2.NewElement("corpus");
		doc2.InsertFirstChild(root2);
	}

	std::cout << "Обрабатываю файл: " << argv[1] << std::endl;

	unsigned int pages = 0;

	XMLElement const* pageElem = root1->FirstChildElement("page");
	while(pageElem != 0)
	{
        XMLElement const* const titleElem = pageElem->FirstChildElement("title");
		XMLElement const* const revisionElem = pageElem->FirstChildElement("revision");

		if(revisionElem)
		{
			XMLElement const* const idElem = revisionElem->FirstChildElement("id");
			XMLElement const* const timestampElem = revisionElem->FirstChildElement("timestamp");
			XMLElement const* const textElem = revisionElem->FirstChildElement("text");

			if(idElem && timestampElem && textElem)
			{
				XMLElement* const elem = doc2.NewElement("page");
				if(elem)
				{
					elem->SetText(textElem->GetText());
					elem->SetAttribute("id", idElem->GetText());
                    elem->SetAttribute("title", (titleElem) ? titleElem->GetText() : "");
					elem->SetAttribute("timestamp", timestampElem->GetText());
					//elem->SetAttribute("bytes", strlen(textElem->GetText()));
					root2->InsertEndChild(elem);
				}

				if(pages % 500 == 0)
					std::cout << '\r' << pages << " статей обработано";
				++pages;
			}
		}

		pageElem = pageElem->NextSiblingElement("page");
	}

	doc2.SaveFile(argv[2]);
	std::cout << std::endl << "Обработка успешно завершена" << std::endl;
	return 0;
}
