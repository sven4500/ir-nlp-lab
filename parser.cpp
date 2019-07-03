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
#include "range.h"

// Класс который хранит словарь для быстрого доступа.
class
{
public:
    bool read(std::ifstream& fin)
    {
        if(!fin)
            return false;

        // Проверяем метку индексного файла.
        fin.seekg(0, std::ios::beg);
        fin.read((char*)&_fileHead, sizeof(_fileHead));
        if(_fileHead._sign != 0xDFDFDFDF || _fileHead._tokenCount == 0)
            return false;

        unsigned int const lookupBytes = _fileHead._tokenCount * 8;
        _lookup.resize(lookupBytes/8);
        fin.read((char*)&_lookup[0], lookupBytes);

        return true;
    }

    unsigned int find(unsigned int termID)const
    {
        for(std::size_t i = 0; i < _lookup.size(); ++i)
            if(_lookup[i].first == termID)
                return _lookup[i].second;
        return 0;
    }

protected:
    std::vector<std::pair<unsigned int, unsigned int>> _lookup;

    // Заголовок файла на всякий случай сохраняем.
    struct
    {
        unsigned int _sign;
        unsigned int _tokenCount;
        unsigned int _unused[2];
    }_fileHead;

}lookup;

// Метод производит преобразование числа и возвращает количество байт которые
// занимает число в сжатом виде.
unsigned int decodeNumber(unsigned char const* in, unsigned int* const pCount)
{
    // В старших двух битах хранится количество байт которыми закодировано число.
    unsigned int const count = (*in) >> 6;
    unsigned int out = (*in) & 0x3f;

    for(unsigned int i = 0; i < count; ++i)
    {
        out <<= 8;
        out |= *(++in);
    }

    if(pCount)
        *pCount = count + 1;

    return out;
}

// Функция возвращает список документов в которых встречается термин termID.
std::vector<unsigned int> getDocID(std::ifstream& fin, unsigned int const termID)
{
	std::streamoff posAt = lookup.find(termID);
	if(posAt == 0)
		return std::vector<unsigned int>();

    #pragma pack(push, 1)
	struct
    {
        unsigned short _sign;
        unsigned short _termID; // нижние 16-бит идентификатора термина
        unsigned short _stride;
        unsigned short _blockBytes;
    }termHead = {};
    #pragma pack(pop)

	fin.seekg(posAt, std::ios::beg);
	fin.read((char*)&termHead, sizeof(termHead));

	// Проверяем правильность заголовка блока описывающего токен:
	// сигнатура должна быть на месте, идентификатор токена должен быть
	// равен запрашиваемому идентификатору и количество документов
	// тожен не может быть равно нулю.
	if(termHead._sign != 0xABAB || termHead._blockBytes == 0 || termHead._stride == 0 || termHead._termID != (termID & 0xffff))
		return std::vector<unsigned int>();

    // Считываем сырые данные блока.
    std::vector<unsigned char> bytes(termHead._blockBytes);
    fin.read((char*)&bytes[0], termHead._blockBytes);

    // Здесь будет храниться идентификаторы документов.
    std::vector<unsigned int> docID;

    unsigned int lastDocID = 0;
    for(std::size_t i = 0, j = 0; i < bytes.size(); ++j)
    {
        unsigned int byteCount = 0;

        if(j % termHead._stride == 0)
        {
            unsigned int skip = decodeNumber(&bytes[i], &byteCount);
            docID.push_back(skip);
        }
        else
        {
            lastDocID += decodeNumber(&bytes[i], &byteCount);
            docID.push_back(lastDocID);
        }

        i += byteCount;
    }

	return docID;
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
    std::vector<unsigned int> vect = getDocID(fin, tokenID);
    return vect;
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
        // Пара <>документ, позиция первого слова цитаты>
        std::map<unsigned long long, unsigned int> hits;

        // i номер токена; j номер документа; k номер словопозиции.
        for(std::size_t i1 = 0, i2 = 1; i2 < vect.size(); ++i2)
        {
            for(std::size_t j1 = 0; j1 < vect[i1].second.size(); ++j1)
            {
                for(std::size_t j2 = 0; j2 < vect[i2].second.size(); ++j2)
                {
                    // Оптимизация. Нет смысла сравнивать словопозиции если слова
                    // находятся в разных документах.
                    if(vect[i2].second[j2].first != vect[i1].second[j1].first)
                        continue;

                    for(std::size_t k1 = 0; k1 < vect[i1].second[j1].second.size(); ++k1)
                    {
                        for(std::size_t k2 = 0; k2 < vect[i2].second[j2].second.size(); ++k2)
                        {
                            unsigned long long const h = ((unsigned long long)vect[i1].second[j1].first << 32) | vect[i1].second[j1].second[k1];
                            if(vect[i2].second[j2].second[k2] - vect[i1].second[j1].second[k1] < distance)
                                ++hits[h];
                        }
                    }
                }
            }
        }

        std::set<unsigned int> uHits;
        for(std::map<unsigned long long, unsigned int>::const_iterator iter = hits.cbegin(); iter != hits.cend(); ++iter)
            if(iter->second == vect.size() - 1)
                uHits.insert((unsigned int)(iter->first >> 32));

        for(std::set<unsigned int>::const_iterator iter = uHits.begin(), end = uHits.end(); iter != end; ++iter)
            result.push_back(*iter);
    }

    return result;
}

std::vector<unsigned int> parseSub(std::ifstream& finIndex, std::ifstream& finPosInd, std::stringstream& expr)
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
			unit = parseSub(finIndex, finPosInd, expr);
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
std::vector<unsigned int> parseFuzzy(std::ifstream& finIndex, std::ifstream& finPosIndex, std::stringstream& expr)
{
    std::vector<unsigned int> docID;
    std::string token;

    while(expr >> token && !token.empty())
    {
        std::vector<unsigned int> temp1 = parseAtom(finIndex, token), temp2;
        std::sort(docID.begin(), docID.end());
        std::sort(temp1.begin(), temp1.end());
        std::set_union(docID.begin(), docID.end(), temp1.begin(), temp1.end(), std::back_inserter(temp2));
        docID.swap(temp2);
    }

    return docID;
}

std::vector<unsigned int> parse(std::ifstream& finInd, std::ifstream& finPosInd, std::ifstream& finTFIDF, char const* const expr)
{
    std::stringstream ss(expr);
    std::vector<unsigned int> tokenID;
    std::string token;
    bool isFuzzy = true;

    // В начале обработки запроса подгружаем в память словарь.
    lookup.read(finInd);

    while(ss >> token && !token.empty())
    {
        // Если в запросе присутствутет хотябы один из нижеперечисленных
        // символов, то запрос считается чётким.
        if(token == "&&" || token == "||" || token == "!" || token == "\"" || token == "\\" || token == "(" || token == ")")
            isFuzzy = false;
        else
            tokenID.push_back(makeTokenID(token));
    }

    // Возвращаем указатель выражения обратно на начало.
    ss.clear();
    ss.seekg(0, std::ios::beg);

    // В зависимости от того был ли запрос чётким обрабатываем его.
    std::vector<unsigned int> docID = (tokenID.size() > 1 && isFuzzy == true) ? parseFuzzy(finInd, finPosInd, ss) : parseSub(finInd, finPosInd, ss);
    //range(docID, tokenID, finTFIDF);
    return docID;
}
