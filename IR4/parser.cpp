#include <unicode/unistr.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <string>
#include <crc32.h>
#include "parser.h"

std::vector<unsigned int> getDocID(std::ifstream& fin, unsigned int const termID)
{
	unsigned int fileHead[4] = {};

	fin.seekg(0, std::ios::beg);
	fin.read((char*)&fileHead, sizeof(fileHead));

	// ѕровер€ем метку индексного файла.
	if(fileHead[0] != 0xABABABAB)
		return std::vector<unsigned int>();

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
		return std::vector<unsigned int>();

	unsigned int tokenHead[4] = {};

	fin.seekg(posAt, std::ios::beg);
	fin.read((char*)tokenHead, sizeof(tokenHead));

	// ѕровер€ем правильность заголовка блока описывающего токен:
	// сигнатура должна быть на месте, идентификатор токена должен быть
	// равен запрашиваемому идентификатору и количество документов
	// тожен не может быть равно нулю.
	if(tokenHead[0] != 0xAAAAAAAA || tokenHead[1] != termID || tokenHead[2] == 0)
		return std::vector<unsigned int>();

	// Ќа вс€кий случай предохран€емс€ от слишком большого количества документов.
	std::size_t const docCount = std::min(tokenHead[2], 2500U);
	std::vector<unsigned int> docID(docCount);

	fin.read((char*)&docID[0], sizeof(unsigned int) * docCount);
	return std::vector<unsigned int>(docID.begin(), docID.end());
}

std::vector<unsigned int> parseAtom(std::ifstream& fin, std::string token)
{
	icu::UnicodeString uniToken(token.c_str(), "UTF8");
	uniToken.toLower();
	token.clear();
	uniToken.toUTF8String(token);
	unsigned int const hash = crc32(0, token.c_str(), token.length());
	return getDocID(fin, hash);
}

std::vector<unsigned int> parseSub(std::ifstream& fin, std::stringstream& expr)
{
	std::vector<unsigned int> result;
	std::string op;

	while(true)
	{
		std::vector<unsigned int> operA;
		std::string token;

		expr >> token;
		if(/*expr.eof() ||*/ token.empty())
			return result;

		if(token == "&&" || token == "||")
			op = token;
		else if(token == "(")
			operA = parseSub(fin, expr);
		else if(token == ")")
			return result;
		else
			operA = parseAtom(fin, token);

		if(!operA.empty())
		{
			std::vector<unsigned int> operB;

			std::sort(operA.begin(), operA.end());
			std::sort(result.begin(), result.end());
			
			if(op == "&&")
				std::set_intersection(result.begin(), result.end(), operA.begin(), operA.end(), std::back_inserter(operB));
			else if(op == "||")
				std::set_union(result.begin(), result.end(), operA.begin(), operA.end(), std::back_inserter(operB));
			else
				operB = operA;

			result = operB;
		}
	}
}

std::vector<unsigned int> parse(std::ifstream& fin, char const* const expr)
{
	std::stringstream ss(expr);
	return parseSub(fin, ss);
}
