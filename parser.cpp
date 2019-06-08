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

// ������� ���������� ������ ���������� � ������� ����������� ������ termID.
std::vector<unsigned int> getDocID(std::ifstream& fin, unsigned int const termID)
{
	unsigned int fileHead[4] = {};

	fin.seekg(0, std::ios::beg);
	fin.read((char*)&fileHead, sizeof(fileHead));

	// ��������� ����� ���������� �����.
	if(fileHead[0] != 0xABABABAB)
		return std::vector<unsigned int>();

	std::streamoff posAt = 0;

	// ��������� �� ������� �������� - ���� ��� ������.
	// TODO: ����� ������������� ����� ����� ������.
	while(fin.tellg() < fileHead[1])
	{
		// �� ������� ��������� �������� ������� � ���� ��� �����.
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

	// ��������� ������������ ��������� ����� ������������ �����:
	// ��������� ������ ���� �� �����, ������������� ������ ������ ����
	// ����� �������������� �������������� � ���������� ����������
	// ����� �� ����� ���� ����� ����.
	if(tokenHead[0] != 0xAAAAAAAA || tokenHead[1] != termID || tokenHead[2] == 0)
		return std::vector<unsigned int>();

	// �� ������ ������ �������������� �� ������� �������� ���������� ����������.
	std::size_t const docCount = std::min(tokenHead[2], 2500U);
	std::vector<unsigned int> docID(docCount);

	fin.read((char*)&docID[0], sizeof(unsigned int) * docCount);
	return std::vector<unsigned int>(docID.begin(), docID.end());
}

// ������� ���������� ����������� ���������� �� ������ ������������ ��� ������� termID.
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

    // ��������� ������� � ������. ��� ����� ����� �������.
    std::vector<std::pair<unsigned int, unsigned int>> lookupTable(fileHead._tokenCount);
    fin.read((char*)&lookupTable[0], fileHead._lookupTableBytes);

    std::pair<unsigned int, unsigned int> lookupRecord(0, 0);

    // ������� � ������� ������ ����������� � �������� ������.
    for(std::size_t i = 0; i < lookupTable.size(); ++i)
    {
        if(lookupTable[i].first == tokenID)
        {
            lookupRecord = lookupTable[i];
            break;
        }
    }

    // ��������� ��� �� ������ ����� ����� � �������.
    if(lookupRecord.first == 0)
        return std::vector<std::pair<unsigned int, std::vector<unsigned int>>>();

    // ��������� ���� ��� ���������� ���� ������� ���������
    // ���������������� � ������� �������.
    fin.seekg(lookupRecord.second, std::ios::beg);

    std::vector<std::pair<unsigned int, std::vector<unsigned int>>> docIDToPos;

    // ��������� ��� ����� ���������� ����������� � ���������� ������.
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

        // ��������� ������������ ��������� ����� ������. �� ������ ������
        // ��������� ��������� ���������, ��-������ � ��������� ������ ����
        // ������ ������� �����, � ���� ��� �� ���, �� ������ ��������� ���
        // � ���������� ������.
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

#pragma pack(push, 1)

// ��������� ��������� �������� � ������� ����������� ������������� �����. ��
// ������ ����� �������� ��������� ������ � �������.
struct DocFreq
{
    unsigned int _docID;                // ������������� ���������
    unsigned short _tokenCount;         // ���������� ��������� ������
    unsigned short _tokenTotalCount;    // ����� ������� � ���������
};

// ��������� ��������� ���� ������ � �������.
struct DictItem
{
    unsigned int _tokenID;
    unsigned int _docCount;
    unsigned int _offBytes;
    unsigned int _unused;
};

#pragma pack(pop)

struct TFIDFPairMetric
{
    unsigned int _tokenID;
    unsigned int _docCount;
    unsigned int _docTotalCount;
    unsigned int _docID;
    unsigned int _tokenCount;
    unsigned int _tokenTotalCount;
};

struct TFIDFMetric
{
    TFIDFMetric(): _tokenID(0), _docCount(0), _docTotalCount(0)
    {}

    // ����� ������������ ������ ������� � ������� ��� ���� ����� - ��������.
    TFIDFPairMetric toPairMetric(unsigned int docID)const
    {
        TFIDFPairMetric metric;
        for(std::size_t i = 0; i < _docFreq.size(); ++i)
        {
            if(_docFreq[i]._docID == docID)
            {
                metric._tokenID = _tokenID;
                metric._docCount = _docCount;
                metric._docTotalCount = _docTotalCount;

                metric._docID = docID;
                metric._tokenCount = _docFreq[i]._tokenCount;
                metric._tokenTotalCount = _docFreq[i]._tokenTotalCount;
                
                return metric;
            }
        }
        return metric;
    }

    unsigned int _tokenID;
    unsigned int _docCount;
    unsigned int _docTotalCount;
    std::vector<DocFreq> _docFreq;
};

TFIDFMetric getTFIDF(std::ifstream& fin, unsigned int tokenID)
{
    TFIDFMetric metric /*= {}*/;
    // ������������� ������ �� ����� ���� ������� ����� ����� �������
    // �������������. ������ ������������� ��������� ������ ����� ���� �
    // ������� ���� ��� �� ���������� ���������� ������������ �����������
    // ���������.
    if(!fin || tokenID == 0)
        return metric;

    struct
    {
        unsigned int _sign;
        unsigned int _tokenCount;
        unsigned int _docCount;
        unsigned int _unused;
    }fileHd;
    fin.read((char*)&fileHd, sizeof(fileHd));

    if(fileHd._sign != 0x8C8C8C8C || fileHd._tokenCount == 0 || fileHd._docCount == 0)
        return metric;

    DictItem dictItem;
    for(unsigned int i = 0; i < fileHd._tokenCount; ++i)
        if(fin.read((char*)&dictItem, sizeof(dictItem)) && dictItem._tokenID == tokenID)
            break;

    // ���� ������� ����� �� ��� ������, �� ���������� ������ �������.
    if(dictItem._tokenID != tokenID || dictItem._docCount == 0)
        return metric;

    metric._tokenID = tokenID;
    metric._docCount = dictItem._docCount;
    metric._docTotalCount = fileHd._docCount;
    metric._docFreq.resize(metric._docCount);

    fin.seekg(dictItem._offBytes, std::ios::beg);
    fin.read((char*)&metric._docFreq[0], sizeof(DocFreq) * metric._docCount);

    return metric;
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
    // ������ ������ ���� ����� - ������. ������ ������ ������ ���� �������� -
    // ������ ������������.
    std::vector<std::pair<unsigned int, std::vector<std::pair<unsigned int, std::vector<unsigned int>>>>> vect;
    unsigned int distance = 0;

    // ��������� �������� ������.
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
            // ���������� ������� �� ��� ������� � �������� �������.
            // ����� ����� ����� ���������� i ����� �������� ���!
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

        // i ����� ������; j ����� ���������; k ����� ������������.
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
                            // ������� ����� ������ ������������� � ������ ����������
                            // � ����� ������� ����� ���������� �����
                            // ���� �� ��������� ������������ ��������.
                            // ����� �������������� ������������ ������ ������ ��� ��� i2 = i1 + 1 ������.
                            if(vect[i2].second[j2].first == vect[i1].second[j1].first &&
                                vect[i2].second[j2].second[k2] - vect[i1].second[j1].second[k1] < distance)
                            {
                                // ��� ��� �������� ���� � ��� �� ����� �� ������ [i1][j1] ��� [i2][j2].
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

std::vector<unsigned int> parseSub(std::ifstream& finIndex, std::ifstream& finPosInd, std::stringstream& expr)
{
	std::vector<unsigned int> result;
	std::string oper;

	while(true)
	{
        std::vector<unsigned int> unit;
		std::string token;

        // ��������� ����� �� ������.
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

// ���������� ��������� ���������.
std::vector<unsigned int> parseFuzzy(std::ifstream& finInd, std::ifstream& finPosInd, std::stringstream& expr)
{
    std::vector<unsigned int> res;
    return res;
}

// ������� ������������ TF-IDF.
void range(std::vector<unsigned int>& docs, std::ifstream& finTFIDF, std::vector<unsigned int> const& tokens)
{
}

std::vector<unsigned int> parse(std::ifstream& finInd, std::ifstream& finPosInd, std::ifstream& finTFIDF, char const* const expr)
{
    std::stringstream ss(expr);
    std::vector<unsigned int> tokenID;
    std::string token;
    bool isFuzzy = true;

    while(ss >> token && !token.empty())
    {
        // ���� � ������� ������������� ������ ���� �� �����������������
        // ��������, �� ������ ��������� ������.
        if(token == "&&" || token == "||" || token == "!" || token == "\"" || token == "\\" || token == "(" || token == ")")
            isFuzzy = false;
        tokenID.push_back(makeTokenID(token));
    }

    // ���������� ��������� ��������� ������� �� ������.
    ss.clear();
    ss.seekg(0, std::ios::beg);

    // � ����������� �� ���� ��� �� ������ ������ ������������ ���.
    std::vector<unsigned int> docID = (tokenID.size() > 1 && isFuzzy == true) ? parseFuzzy(finInd, finPosInd, ss) : parseSub(finInd, finPosInd, ss);
    range(docID, finTFIDF, tokenID);
    return docID;
}
