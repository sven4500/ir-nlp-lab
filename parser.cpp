#include <unicode/unistr.h>
#include <vector>
#include <iostream>
#include <iterator>
#include <algorithm> // sort
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <crc32.h>
#include "parser.h"
#include "TFIDFRange.h"
#include "zoneRange.h"
#include "posIndexReader.h"

unsigned int skip = 0;
unsigned int skipCount = 0;

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

// Эта структура хранит список идентификаторов и рассотяние межде маркерами
// прыжков. Если _stride < 2 значит маркеров прыжков нет.
struct DocIDList
{
    DocIDList(): _list(0), _stride(0)
    {}

    DocIDList(std::vector<unsigned int> const& list, unsigned int const stride): _list(list), _stride(stride)
    {}

    std::vector<unsigned int> _list;
    unsigned int _stride;
};

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
DocIDList getDocID(std::ifstream& fin, unsigned int const termID)
{
    DocIDList list;

	std::streamoff posAt = lookup.find(termID);
	if(posAt == 0)
        return list;

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
	if(termHead._sign != 0xABAB || termHead._blockBytes == 0 || termHead._stride < 2 || termHead._termID != (termID & 0xffff))
		return list;

    list._stride = termHead._stride;

    // Считываем сырые данные блока.
    std::vector<unsigned char> bytes(termHead._blockBytes);
    fin.read((char*)&bytes[0], termHead._blockBytes);

    unsigned int lastDocID = 0;
    for(std::size_t i = 0, j = 0; i < bytes.size(); ++j)
    {
        unsigned int byteCount = 0;

        if(j % termHead._stride == 0)
        {
            unsigned int skip = decodeNumber(&bytes[i], &byteCount);
            list._list.push_back(skip);
        }
        else
        {
            lastDocID += decodeNumber(&bytes[i], &byteCount);
            list._list.push_back(lastDocID);
        }

        i += byteCount;
    }

	return list;
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

std::vector<unsigned int> unite(std::vector<unsigned int> const& a, std::vector<unsigned int> const& b, unsigned int const as = 0, unsigned int const bs = 0)
{
    std::set<unsigned int> c;
    for(std::size_t i = 0; i < a.size(); ++i)
        if(i % as != 0 && as > 1)
            c.insert(a[i]);
    for(std::size_t i = 0; i < b.size(); ++i)
        if(i % bs != 0 && bs > 1)
            c.insert(b[i]);
    return std::vector<unsigned int>(c.begin(), c.end());
}

DocIDList unite(DocIDList const& a, DocIDList const& b)
{
    return DocIDList(unite(a._list, b._list, a._stride, b._stride), 0);
}

// as, bs расстояние между метками для вектора a и b соответственно.
std::vector<unsigned int> intersect(std::vector<unsigned int> const& a, std::vector<unsigned int> const& b, unsigned int const as = 0, unsigned int const bs = 0)
{
    std::vector<unsigned int> c;
    std::size_t const size = std::min(a.size(), b.size());
    for(std::size_t i = 0, j = 0; i < size && j < size; )
    {
        if(i % as == 0 && as > 1 && j % bs == 0 && bs > 1)
        {
            ++i;
            ++j;
        }
        else if(i % as == 0 && as > 1)
        {
            i += (a[i] < b[j]) ? as : 1;

            if(a[i] < b[j])
            {
                skip += as;
                skipCount += 1;
            }
        }
        else if(j % bs == 0 && bs > 1)
        {
            j += (a[i] > b[j]) ? bs : 1;

            if(a[i] > b[j])
            {
                skip += bs;
                skipCount += 1;
            }
        }
        else if(a[i] < b[j])
        {
            ++i;
        }
        else if(a[i] > b[j])
        {
            ++j;
        }
        else if(a[i] == b[j])
        {
            c.push_back(a[j]);
            ++i;
            ++j;
        }
    }
    return c;
}

DocIDList intersect(DocIDList const& a, DocIDList const& b)
{
    return DocIDList(intersect(a._list, b._list, a._stride, b._stride), 0);
}

DocIDList parseAtom(std::ifstream& fin, std::string const& token)
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
            std::pair<unsigned int, std::vector<std::pair<unsigned int, std::vector<unsigned int>>>> const p(tokenID, PosIndexReader::getDocIDToPos(tokenID, finPosindex));
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

DocIDList parseSub(std::ifstream& finIndex, std::ifstream& finPosInd, std::stringstream& expr)
{
	DocIDList result;
	std::string oper;

	while(true)
	{
        DocIDList unit;
		std::string token;

        // Считываем токен из потока.
        expr >> token;

		if(/*expr.eof() ||*/ token.empty())
			return result;

		if(token == "&&" || token == "||")
        {
			oper = token;
        }
		else if(token == "(")
        {
			unit = parseSub(finIndex, finPosInd, expr);
        }
		else if(token == ")")
        {
			return result;
        }
        else if(token == "\"")
        {
            unit._list = parseCitation(finIndex, finPosInd, expr);
            unit._stride = 0;
        }
		else
        {
			unit = parseAtom(finIndex, token);
        }

        if(!unit._list.empty())
        {
            DocIDList temp;

		    if(oper == "&&")
            {
                temp = intersect(result, unit);
            }
		    else if(oper == "||")
            {
                temp = unite(result, unit);
            }
            else
            {
                temp = unit;
            }

            result = temp;
        }
	}
}

// Обработчик нечёткого выражения.
DocIDList parseFuzzy(std::ifstream& finIndex, std::ifstream& finPosIndex, std::stringstream& expr)
{
    DocIDList list;
    std::string token;

    while(expr >> token && !token.empty())
    {
        DocIDList const temp = parseAtom(finIndex, token);
        list = unite(list, temp);
    }

    return list;
}

// Предикат для сортировки вектора пар <документ, ранг>. Возвращает true если
// элемент p1 должен находится в списке до p2.
bool pred(std::pair<unsigned int, double> const& p1, std::pair<unsigned int, double> const& p2)
{
    return p1.second > p2.second;
}

std::vector<unsigned int> parse(char const* const expr, std::ifstream& finInd, std::ifstream& finPosInd, std::ifstream& finTFIDF, std::ifstream& finZoneInd)
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
    DocIDList list = (tokenID.size() > 1 && isFuzzy == true) ? parseFuzzy(finInd, finPosInd, ss) : parseSub(finInd, finPosInd, ss);

    std::vector<unsigned int> docID;
    docID.reserve(list._list.size());
    for(std::size_t i = 0; i < list._list.size(); ++i)
        if(list._stride < 2 || (i % list._stride != 0 && list._stride >= 2))
            docID.push_back(list._list[i]);

    // Производим ранжирование списка документов.
    {
        std::vector<std::pair<unsigned int, double>> range0;
        std::vector<std::pair<unsigned int, double>> const range1 = TFIDFRange(docID, tokenID, finTFIDF);
        std::vector<std::pair<unsigned int, double>> const range2 = zoneRange(docID, tokenID, finZoneInd);

        range0.resize(docID.size());
        for(std::size_t i = 0; i < docID.size(); ++i)
        {
            range0[i].first = docID[i];
            range0[i].second = (range1[i].second + range2[i].second) / 2.0;
        }

        std::sort(range0.begin(), range0.end(), pred);
        for(std::size_t i = 0; i < docID.size(); ++i)
            docID[i] = range0[i].first;
    }

    return docID;
}
