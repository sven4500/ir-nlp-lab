#include <unicode/unistr.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <crc32.h>
#include "parser.h"

// ‘ункци€ возвращает список документов в которых встречаетс€ термин termID.
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

// ‘ункци€ возвращает отображение документов на список словопозиций дл€ термина termID.
std::map<unsigned int, std::vector<unsigned int>> getDocIDToPos(std::ifstream& fin, unsigned int tokenID)
{
    fin.seekg(0, std::ios::beg);

    // 0: метка заголовка; 1: количество токенов; 2: размер словар€.
    struct
    {
        unsigned int _sign;
        unsigned int _tokenCount;
        unsigned int _lookupTableBytes;
        unsigned int _reserved;
    }fileHead = {};

    fin.read((char*)&fileHead, sizeof(fileHead));
    if(fileHead._sign != 0xBCBCBCBC || fileHead._tokenCount * 8 != fileHead._lookupTableBytes)
        return std::map<unsigned int, std::vector<unsigned int>>();

    // —читываем словарь в пам€ть. “ак поиск будет быстрее.
    std::vector<std::pair<unsigned int, unsigned int>> lookupTable(fileHead._tokenCount);
    fin.read((char*)&lookupTable[0], fileHead._lookupTableBytes);

    // Ќаходим в словаре запись относ€щуюс€ к искомому токену.
    std::pair<unsigned int, unsigned int> lookupRecord(0, 0);
    for(std::size_t i = 0; i < lookupTable.size(); ++i)
    {
        if(lookupTable[i].first == tokenID)
        {
            lookupRecord = lookupTable[i];
            break;
        }
    }

    // ѕровер€ем был ли найден такой токен в словаре.
    if(lookupRecord.first == 0)
        return std::map<unsigned int, std::vector<unsigned int>>();

    // —мещаемс€ туда где расположен блок первого документа
    // ассоциированного с искомым токеном.
    fin.seekg(lookupRecord.second, std::ios::beg);

    // —читываем все блоки документов относ€щиес€ к выбранному токену.
    std::map<unsigned int, std::vector<unsigned int>> docIDToPos;
    while(true)
    {
        #pragma pack(1)
        struct{
            unsigned int _sign;
            unsigned int _tokenID;
            unsigned int _docID;
            unsigned int _posCount;
        }head = {};

        fin.read((char*)&head, sizeof(head));

        // ѕровер€ем правильность заголовка одной записи. ¬о первых должна
        // совпадать сигнатура заголовка, во-вторых в заголовке должен быть
        // указан искомый ткоен, а если это не так, то запись относитс€ уже
        // к следующему токену.
        if(head._sign != 0x9C9C9C9C || head._tokenID != tokenID)
            break;

        if(head._posCount != 0)
        {
            std::vector<unsigned int>& vect = docIDToPos[head._docID];
            vect.resize(head._posCount);
            fin.read((char*)&vect[0], sizeof(int) * head._posCount);
        }
    }

    return docIDToPos;
}

unsigned int makeTokenID(std::string token)
{
	icu::UnicodeString uniToken(token.c_str(), "UTF8");
	uniToken.toLower();
	token.clear();
	uniToken.toUTF8String(token);
	unsigned int const hash = crc32(0, token.c_str(), token.length());
	return hash;
}

std::vector<unsigned int> parseAtom(std::ifstream& fin, std::string const& token)
{
	unsigned int const tokenID = makeTokenID(token);
	return getDocID(fin, tokenID);
}

std::vector<unsigned int> parseCitation(std::ifstream& finIndex, std::ifstream& finPosindex, std::stringstream& ss)
{
    std::vector<std::map<unsigned int, std::vector<unsigned int>>> vect;
    unsigned int distance = 0;

    // —читываем цитатный запрос.
    {
        std::string token;

        while(ss >> token && token != "\"")
        {
            unsigned int const tokenID = makeTokenID(token);
            vect.push_back(getDocIDToPos(finPosindex, tokenID));
        }

        if(ss >> token && !token.empty())
        {
            if(token == "/")
            {
                ss >> distance;
            }
            else
            {
                // ¬озвращаем обратно то что считали в обратном пор€дке.
                // «десь мы точно знаем что токен не пустой.
                for(int i = token.size() - 1; i >= 0; --i)
                    ss.putback(token[i]);
                distance = vect.size();
            }
        }
    }

    if(vect.empty())
        return std::vector<unsigned int>();

    std::vector<unsigned int> result;

    {
        // ¬ыбираем первое слово цитаты.
        std::map<unsigned int, std::vector<unsigned int>> const& m1 = vect[0];
        for(std::size_t i = 0; i < vect.size(); ++i)
        {
            for(std::size_t i = 0; i < vect.size(); ++i)
            {

            }
        }
    }

    return std::vector<unsigned int>();
}

std::vector<unsigned int> parseSub(std::ifstream& finIndex, std::ifstream& finPosindex, std::stringstream& expr)
{
	std::vector<unsigned int> result;
	std::string oper;

	while(true)
	{
        std::vector<unsigned int> unit;
		std::string token;

        // —читываем токен из потока.
        expr >> token;

		if(/*expr.eof() ||*/ token.empty())
			return result;

		if(token == "&&" || token == "||")
			oper = token;
		else if(token == "(")
			unit = parseSub(finIndex, finPosindex, expr);
		else if(token == ")")
			return result;
        else if(token == "\"")
            unit = parseCitation(finIndex, finPosindex, expr);
		else
			unit = parseAtom(finIndex, token);

        if(!unit.empty())
        {
            std::vector<unsigned int> temp;

		    std::sort(unit.begin(), unit.end());
		    std::sort(result.begin(), result.end());

		    if(oper == "&&")
			    std::set_intersection(result.begin(), result.end(), unit.begin(), unit.end(), std::back_inserter(temp));
		    else if(oper == "||")
			    std::set_union(result.begin(), result.end(), unit.begin(), unit.end(), std::back_inserter(temp));
            else
                temp = unit;

            result = temp;
        }
	}
}

std::vector<unsigned int> parse(std::ifstream& finIndex, std::ifstream& finPosindex, char const* const expr)
{
	std::stringstream ss(expr);
	return parseSub(finIndex, finPosindex, ss);
}
