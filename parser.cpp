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

// Функция возвращает список документов в которых встречается термин termID.
std::vector<unsigned int> getDocID(std::ifstream& fin, unsigned int const termID)
{
	unsigned int fileHead[4] = {};

	fin.seekg(0, std::ios::beg);
	fin.read((char*)&fileHead, sizeof(fileHead));

	// Проверяем метку индексного файла.
	if(fileHead[0] != 0xABABABAB)
		return std::vector<unsigned int>();

	std::streamoff posAt = 0;

	// Двигаемся по таблице индексов - ищем хэш токена.
	// TODO: здесь потенциальное узкое место поиска.
	while(fin.tellg() < fileHead[1])
	{
		// По очереди считываем элементы таблицы и ищем наш токен.
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

	// Проверяем правильность заголовка блока описывающего токен:
	// сигнатура должна быть на месте, идентификатор токена должен быть
	// равен запрашиваемому идентификатору и количество документов
	// тожен не может быть равно нулю.
	if(tokenHead[0] != 0xAAAAAAAA || tokenHead[1] != termID || tokenHead[2] == 0)
		return std::vector<unsigned int>();

	// На всякий случай предохраняемся от слишком большого количества документов.
	std::size_t const docCount = std::min(tokenHead[2], 2500U);
	std::vector<unsigned int> docID(docCount);

	fin.read((char*)&docID[0], sizeof(unsigned int) * docCount);
	return std::vector<unsigned int>(docID.begin(), docID.end());
}

// Функция возвращает отображение документов на список словопозиций для термина termID.
std::vector<std::pair<unsigned int, std::vector<unsigned int>>> getDocIDToPos(std::ifstream& fin, unsigned int tokenID)
{
    fin.seekg(0, std::ios::beg);

    #pragma pack(1)
    struct
    {
        unsigned int _sign;
        unsigned int _tokenCount;
        unsigned int _lookupTableBytes;
        unsigned int _reserved;
    }fileHead = {};

    fin.read((char*)&fileHead, sizeof(fileHead));
    if(fileHead._sign != 0xBCBCBCBC || fileHead._tokenCount * 8 != fileHead._lookupTableBytes)
        return std::vector<std::pair<unsigned int, std::vector<unsigned int>>>();

    // Считываем словарь в память. Так поиск будет быстрее.
    std::vector<std::pair<unsigned int, unsigned int>> lookupTable(fileHead._tokenCount);
    fin.read((char*)&lookupTable[0], fileHead._lookupTableBytes);

    std::pair<unsigned int, unsigned int> lookupRecord(0, 0);

    // Находим в словаре запись относящуюся к искомому токену.
    for(std::size_t i = 0; i < lookupTable.size(); ++i)
    {
        if(lookupTable[i].first == tokenID)
        {
            lookupRecord = lookupTable[i];
            break;
        }
    }

    // Проверяем был ли найден такой токен в словаре.
    if(lookupRecord.first == 0)
        return std::vector<std::pair<unsigned int, std::vector<unsigned int>>>();

    // Смещаемся туда где расположен блок первого документа
    // ассоциированного с искомым токеном.
    fin.seekg(lookupRecord.second, std::ios::beg);

    std::vector<std::pair<unsigned int, std::vector<unsigned int>>> docIDToPos;

    // Считываем все блоки документов относящиеся к выбранному токену.
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

        // Проверяем правильность заголовка одной записи. Во первых должна
        // совпадать сигнатура заголовка, во-вторых в заголовке должен быть
        // указан искомый ткоен, а если это не так, то запись относится уже
        // к следующему токену.
        if(head._sign != 0x9C9C9C9C || head._tokenID != tokenID)
            break;

        if(head._posCount != 0)
        {
            std::pair<unsigned int, std::vector<unsigned int>> p(head._docID, head._posCount);
            fin.read((char*)&p.second[0], sizeof(int) * head._posCount);
            docIDToPos.push_back(p);
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
    // Вектор хранит пары токен - вектор. Второй вектор хранит пары документ -
    // вектор словопозиций.
    std::vector<std::pair<unsigned int, std::vector<std::pair<unsigned int, std::vector<unsigned int>>>>> vect;
    unsigned int distance = 0;

    // Считываем цитатный запрос.
    {
        std::string token;

        while(ss >> token && token != "\"")
        {
            unsigned int const tokenID = makeTokenID(token);
            std::pair<unsigned int, std::vector<std::pair<unsigned int, std::vector<unsigned int>>>> const p(tokenID, getDocIDToPos(finPosindex, tokenID));
            vect.push_back(p);
        }

        if(ss >> token && token == "/")
        {
            ss >> distance;
        }
        else
        {
            // Возвращаем обратно то что считали в обратном порядке.
            // Очень важно чтобы переменная i имела знаковый тип!
            for(int i = token.size() - 1; i >= 0; --i)
                ss.putback(token[i]);
            distance = vect.size();
        }
    }

    if(vect.empty())
        return std::vector<unsigned int>();

    std::vector<unsigned int> result;

    {
        std::map<unsigned int, unsigned int> hits;

        // i номер токена; j номер документа; k номер словопозиции.
        for(std::size_t i1 = 0, i2 = 1; i2 < vect.size(); ++i1, ++i2)
        {
            for(std::size_t j1 = 0; j1 < vect[i1].second.size(); ++j1)
            {
                for(std::size_t j2 = 0; j2 < vect[i2].second.size(); ++j2)
                {
                    for(std::size_t k1 = 0; k1 < vect[i1].second[j1].second.size(); ++k1)
                    {
                        for(std::size_t k2 = 0; k2 < vect[i2].second[j2].second.size(); ++k2)
                        {
                            // Смотрим чтобы токены располагались в разных документах
                            // а после смотрим чтобы расстояние между
                            // ними не превышало определённого значения.
                            // Здесь гарантированно сравниваются разные токены так как i2 = i1 + 1 всегда.
                            if(vect[i2].second[j2].first == vect[i1].second[j1].first &&
                                vect[i2].second[j2].second[k2] - vect[i1].second[j1].second[k1] < distance)
                            {
                                // Так как документ один и тот же здесь не варжно [i1][j1] или [i2][j2].
                                ++hits[vect[i1].second[j1].first];
                            }
                        }
                    }
                }
            }
        }

        for(std::map<unsigned int, unsigned int>::const_iterator iter = hits.cbegin(); iter != hits.cend(); ++iter)
            if(iter->second >= vect.size() - 1)
                result.push_back(iter->first);
    }

    return result;
}

std::vector<unsigned int> parseSub(std::ifstream& finIndex, std::ifstream& finPosInd, std::ifstream& finTFIDF, std::stringstream& expr)
{
	std::vector<unsigned int> result;
	std::string oper;

	while(true)
	{
        std::vector<unsigned int> unit;
		std::string token;

        // Считываем токен из потока.
        expr >> token;

		if(/*expr.eof() ||*/ token.empty())
			return result;

		if(token == "&&" || token == "||")
			oper = token;
		else if(token == "(")
			unit = parseSub(finIndex, finPosInd, finTFIDF, expr);
		else if(token == ")")
			return result;
        else if(token == "\"")
            unit = parseCitation(finIndex, finPosInd, expr);
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

// Обработчик нечёткого выражения.
std::vector<unsigned int> parseFuzzy(std::ifstream& finInd, std::ifstream& finPosInd, std::ifstream& finTFIDF, std::stringstream& expr)
{
    std::vector<unsigned int> res;
    return res;
}

std::vector<unsigned int> parse(std::ifstream& finInd, std::ifstream& finPosInd, std::ifstream& finTFIDF, char const* const expr)
{
    std::stringstream ss(expr);
    std::string token;
    unsigned int tokenCount = 0;
    bool isFuzzy = true;

    while(ss >> token && !token.empty())
    {
        // Если в запросе присутствутет хотябы один из нижеперечисленных
        // символов, то запрос считается чётким.
        if(token == "&&" || token == "||" || token == "\"" || token == "(" || token == "!")
        {
            isFuzzy = false;
            break;
        }
        ++tokenCount;
    }

    // Возвращаем указатель выражения обратно на начало.
    ss.clear();
    ss.seekg(0, std::ios::beg);

    // В зависимости от того был ли запрос чётким обрабатываем его.
    return (tokenCount > 1 && isFuzzy == true) ? parseFuzzy(finInd, finPosInd, finTFIDF, ss) : parseSub(finInd, finPosInd, finTFIDF, ss);
}
