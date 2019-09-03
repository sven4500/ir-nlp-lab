#include <cmath> // log
#include <algorithm> // sort
#include "TFIDFRange.h"

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

// ��������� ��������� ���� <�����, ��������>.
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
        TFIDFPairMetric metric = {};
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

    fin.seekg(0, std::ios::beg);

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

// �������� ��� ���������� ������� ��� <��������, ����>.
/*bool pred(std::pair<unsigned int, double> const& p1, std::pair<unsigned int, double> const& p2)
{
    // ���������� true ���� ������� p1 ������ ��������� � ������ �� p2.
    return p1.second > p2.second;
}*/

// ������� ������������ TF-IDF.
std::vector<std::pair<unsigned int, double>> TFIDFRange(std::vector<unsigned int>& docID, std::vector<unsigned int> const& tokenID, std::ifstream& fin)
{
    if(docID.empty() || tokenID.empty())
        return std::vector<std::pair<unsigned int, double>>();

    std::vector<std::vector<TFIDFPairMetric>> metrics(docID.size(), std::vector<TFIDFPairMetric>(tokenID.size()));

    // �������� ������� ������ (��������� ��� ��������� ���������).
    {
        std::vector<TFIDFMetric> temp(tokenID.size());

        // �������� TF-IDF ������� �� ������ �����. ������� ��������� ���
        // ��������� ��������� � ������ ������� ������ �����.
        for(std::size_t i = 0; i < temp.size(); ++i)
            temp[i] = getTFIDF(fin, tokenID[i]);

        // ��������������� ������� � ���� <�����, ��������>.
        for(std::size_t i = 0; i < docID.size(); ++i)
            for(std::size_t j = 0; j < tokenID.size(); ++j)
                metrics[i][j] = temp[j].toPairMetric(docID[i]);
    }

    // ������ ��� <��������, ����>. ����� ���� ��� �� ����� ��������
    // ������� ��������� ����� ��������������� ����������� ����.
    // ������������ ���� ������ ������� ������������� ������ ����������.
    std::vector<std::pair<unsigned int, double>> rangePair(docID.size(), std::pair<unsigned int, double>(0, 0.0));

    for(std::size_t i = 0; i < docID.size(); ++i)
    {
        rangePair[i].first = docID[i];

        for(std::size_t j = 0; j < tokenID.size(); ++j)
        {
            double const TF = (metrics[i][j]._tokenTotalCount > 0) ? (double)metrics[i][j]._tokenCount / metrics[i][j]._tokenTotalCount : 0.0;
            double const IDF = (metrics[i][j]._docTotalCount > 0 && metrics[i][j]._docCount > 0) ? log((double)metrics[i][j]._docTotalCount / metrics[i][j]._docCount) : 0.0;
            rangePair[i].second += TF * IDF;
        }
    }

    /*std::sort(rangePair.begin(), rangePair.end(), pred);

    for(std::size_t i = 0; i < docID.size(); ++i)
        docID[i] = rangePair[i].first;*/

    return rangePair;
}
