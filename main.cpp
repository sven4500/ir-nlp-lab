#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <locale>
#include <unicode/unistr.h>
#include <tinyxml2.h>
using namespace tinyxml2;

inline void wipe_one(std::string& str, std::string const& seq)
{
	std::size_t pos = 0;
	while((pos = str.find(seq, pos)) != std::string::npos)
		str.replace(pos++, seq.length(), " ");
}

void wipe_all(std::string& str)
{
	static std::string const seq[] = {
		"\xE2\x80\x93", // EN DASH
		"\xE2\x80\x94", // EM DASH
		"\xE2\x88\x92", // MINUS SIGN
		"\xC2\xA0", // NON-BREAK SPACE
		"\xC2\xAB", // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
		"\xC2\xBB" // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
		/*"\xE2\x84\x96" // NUMERO SIGN*/
	};
	static unsigned int const size = sizeof(seq) / sizeof(std::string);
	for(unsigned int i = 0; i < size; ++i)
		wipe_one(str, seq[i]);
}

bool tokenize(XMLElement const* elem, std::set<std::string>& tokens)
{
	static std::string const delim(" ,.!?@$#&%_/*-+|<>(){}[]:;=`\\\"\'");

	// Исключаем дополнительные символы UTF-8.
	std::string line = elem->GetText();
	wipe_all(line);

	std::size_t pos = 0;
	std::size_t end = 0;

	while((end = line.find_first_of(delim, pos)) != std::string::npos)
	{
		std::size_t const size = end - pos;
		if(size > 1)
		{
			// Здесь всё равно могут затесаться знаки препинания и
			// другие символы из UTF8. Проблема в том что эти знаки
			// кодируются уде 2, 3 или 4 байтами. Для их отделения
			// нужно использовать уже немного другой метод.
			std::string const token(line.substr(pos, size));
				
			icu::UnicodeString uniToken(token.c_str(), "UTF8");
			uniToken.toLower();

			std::string finalToken;
			uniToken.toUTF8String(finalToken);

			if(!isdigit((unsigned char)finalToken[0]))
				tokens.insert(finalToken);
		}
		pos = end + 1;
	}

	return !tokens.empty();
}

bool addTokens(XMLElement* const elem, std::set<std::string> const& tokens)
{
	std::stringstream ss;
	std::set<std::string>::const_iterator iter = tokens.cbegin();
	std::set<std::string>::const_iterator const end = tokens.cend();
	while(iter != end)
	{
		ss << *iter << std::endl;
		++iter;
	}
	elem->SetText(ss.str().c_str());
	return true;
}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");

	if(argc != 3)
	{
		std::cout << "NLP1.exe [входной XML файл] [выходной XML файл]" << std::endl;
		return -1;
	}

	XMLError err = XML_SUCCESS;

	XMLDocument doc1;
	err = doc1.LoadFile(argv[1]);
	if(err != XML_SUCCESS)
	{
		std::cout << "Не могу открыть файл для чтения: " << argv[1] << std::endl;
		return 1;
	}

	XMLElement const* const root1 = doc1.FirstChildElement();
	if(!root1)
	{
		std::cout << "Не могу найти корень дерева XML" << std::endl;
		return 1;
	}

	XMLDocument doc2;
	doc2.LoadFile(argv[2]);

	XMLElement* root2 = doc2.FirstChildElement();
	if(!root2)
	{
		root2 = doc2.NewElement("tokens");
		doc2.InsertFirstChild(root2);
	}

	std::cout << "Обрабатываю файл: " << argv[1] << std::endl;

	unsigned int count = 0;

	XMLElement const* pageElem = root1->FirstChildElement("page");
	while(pageElem != 0)
	{
		XMLElement* const elem = doc2.NewElement("page");
		if(elem)
		{
			std::set<std::string> tokens;
			tokenize(pageElem, tokens);
			addTokens(elem, tokens);

			elem->SetAttribute("id", pageElem->Attribute("id", 0));
			elem->SetAttribute("timestamp", pageElem->Attribute("timestamp", 0));
			root2->InsertEndChild(elem);
		}

		if(count % 1000 == 0)
			std::cout << "\rСтатей обработано: " << count;
		++count;

		pageElem = pageElem->NextSiblingElement("page");
	}

	std::cout << std::endl << "Обработка успешно завершена" << std::endl;
	doc2.SaveFile(argv[2]);
	return 0;
}
