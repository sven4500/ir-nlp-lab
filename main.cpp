#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <ctime>
#include <tinyxml2.h>
#include <crc32.h>
using namespace tinyxml2;

std::size_t meanChars = 0;

// Добавляет идентификатор документа docID для всех терминов содержащихся в
// этом документе.
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
			meanChars = (meanChars + size) / 2;
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

	// Количество всех терминов.
	std::size_t const termCount = termToDocID.size();

	// Словарь: инициализируем вектор и вычисляем размер словаря в байтах.
	std::vector<unsigned int> lookupTable(2 * termCount, 0);
	unsigned int const lookupTableBytes = lookupTable.size() * sizeof(int);
	
	// Записываем заголовок файла.
	unsigned int const fileHeader[4] = {0xABABABAB, sizeof(fileHeader) + lookupTableBytes};
	fout.write((char*)fileHeader, sizeof(fileHeader));

	// Пока прокускаем словарь. Запишем его после.
	fout.seekp(lookupTableBytes, std::ios::cur);

	std::map<unsigned int, std::vector<unsigned int> >::const_iterator iter = termToDocID.cbegin();
	//std::map<unsigned int, std::vector<unsigned int> >::const_iterator const end = termToDocID.cend();

	std::cout << "Записываю файл..." << std::endl;

	for(std::size_t i = 0; i < termCount; ++i)
	{
		unsigned int const& termID = iter->first;
		std::vector<unsigned int> const& docID = iter->second;

		// Сохраняем информацию о смещении темина в словарь.
		lookupTable[2*i+0] = termID;
		lookupTable[2*i+1] = (unsigned int)fout.tellp();

		// Заголовок термина. Каждому списку предшевствует этот заголовок.
		unsigned int const termHeader[4] = {0xAAAAAAAA, termID, docID.size()};

		// Записываем информацию о текмине.
		fout.write((char*)termHeader, sizeof(termHeader));
		if(!docID.empty())
			fout.write((char*)&docID[0], docID.size() * sizeof(int));

		++iter;

		if(i % 10000 == 0)
		{
			std::cout << "\rТерминов записано: " << i << '/' << termCount;
			fout.flush();
		}
	}

	std::cout << "\nЗаписываю словарь..." << std::endl;

	// Записываем словарь в файл.
	fout.seekp(sizeof(fileHeader), std::ios::beg);
	fout.write((char*)&lookupTable[0], lookupTableBytes);

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

	std::clock_t const timeBegin = clock();
	while(pageElem != 0)
	{
		updateIndex(index, pageElem);
		pageElem = pageElem->NextSiblingElement();
		++pages;

		if(pages % 500 == 0)
			std::cout << "\rСтатей обработано: " << pages;

		#if defined(_DEBUG)
		if(pages == 1000)
			break;
		#endif
	}
	std::clock_t const timeEnd = clock();

	std::cout << "\nФормирование индексного файла завершено.\n"
		<< "Средняя длина термина: " << meanChars << std::endl
		<< "Количество терминов: " << index.size() << std::endl
		<< "Время: " << (timeEnd - timeBegin) / CLOCKS_PER_SEC << " сек." << std::endl;

	writeIndex(index, argv[2]);
	return 0;
}
