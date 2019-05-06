#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <clocale>
#include <string>
#include <vector>
#include <set>
#include <unicode/unistr.h>
#include <crc32.h>

std::set<unsigned int> getDocID(std::ifstream& fin, unsigned int const termID)
{
	unsigned int fileHead[4] = {};

	fin.seekg(0, std::ios::beg);
	fin.read((char*)&fileHead, sizeof(fileHead));

	// ѕровер€ем метку индексного файла.
	if(fileHead[0] != 0xABABABAB)
		return std::set<unsigned int>();

	std::streamoff posAt = 0;

	// ƒвигаемс€ по таблице индексов - ищем хэш токена.
	// TODO: здесь потенциальное узкое место поиска.
	while(fin.tellg() < fileHead[1])
	{
		// ѕо очереди считываем элементы таблицы и ищем наш токен.
		unsigned int tabElem[2] = {};
		fin.read((char*)tabElem, sizeof(tabElem));

		if(tabElem[0] == termID)
		{
			posAt = tabElem[1];
			break;
		}
	}

	if(posAt == 0)
		return std::set<unsigned int>();

	unsigned int tokenHead[4] = {};

	fin.seekg(posAt, std::ios::beg);
	fin.read((char*)tokenHead, sizeof(tokenHead));

	// ѕровер€ем правильность заголовка блока описывающего токен:
	// сигнатура должна быть на месте, идентификатор токена должен быть
	// равен запрашиваемому идентификатору и количество документов
	// тожен не может быть равно нулю.
	if(tokenHead[0] != 0xAAAAAAAA || tokenHead[1] != termID || tokenHead[2] == 0)
		return std::set<unsigned int>();

	// Ќа вс€кий случай предохран€емс€ от слишком большого количества документов.
	std::size_t const docCount = std::min(tokenHead[2], 2500U);
	std::vector<unsigned int> docID(docCount);

	fin.read((char*)&docID[0], sizeof(unsigned int) * docCount);
	return std::set<unsigned int>(docID.begin(), docID.end());
}

int main(int argc, char** argv)
{
	std::setlocale(LC_CTYPE, "Russian");

	if(argc != 3)
	{
		std::cout << "IR4.exe индекс запрос" << std::endl;
		return 1;
	}

	std::ifstream fin;
	fin.open(argv[1], std::ios::binary);

	if(!fin)
	{
		std::cout << "Ќе удалось открыть индекс." << std::endl;
		return -1;
	}

	{
		icu::UnicodeString uniToken(argv[2], "UTF8");
		uniToken.toLower();

		std::string token;
		uniToken.toUTF8String(token);

		unsigned int const hash = crc32(0, token.c_str(), token.length());

		std::set<unsigned int> docID;
		docID = getDocID(fin, hash);

		for(std::set<unsigned int>::const_iterator iter = docID.cbegin(); iter != docID.cend(); ++iter)
			std::cout << *iter << ' ';
	}

	return 0;
}
